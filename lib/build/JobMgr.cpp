/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/JobMgr.h"

#include "mint/eval/Evaluator.h"

#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"

namespace mint {

// -------------------------------------------------------------------------
// Job
// -------------------------------------------------------------------------

void Job::begin() {
  Evaluator eval(_target->definition());
  Object * targetObj = _target->definition();
  Node * actions = eval.attributeValue(targetObj, "actions");
  Node * outputDir = eval.attributeValue(targetObj, "output_dir");
  M_ASSERT(actions != NULL);
  M_ASSERT(actions->nodeKind() == Node::NK_LIST);

  Oper * actionList = static_cast<Oper *>(actions);
  _actions.assign(actionList->args().begin(), actionList->args().end());

  if (!outputDir->isUndefined()) {
    M_ASSERT(outputDir->nodeKind() == Node::NK_STRING);
    _outputDir = static_cast<String *>(outputDir);
  }

  _target->setState(Target::BUILDING);
  runNextAction();
}

void Job::runNextAction() {
  while (!_actions.empty()) {
    Node * action = _actions.front();
    _actions.erase(_actions.begin()); // SmallVector has no pop_front().
    switch (action->nodeKind()) {
      case Node::NK_ACTION_COMMAND: {
        if (!_outputDir) {
          diag::error(_target->definition()->location())
              << "No output directory specified for target.";
          break;
        }
        Oper * command = static_cast<Oper *>(action);
        M_ASSERT(command->size() == 2);
        String * program = String::cast(command->arg(0));
        Oper * cargs = static_cast<Oper *>(command->arg(1));
        SmallVector<StringRef, 32> args;
        args.reserve(cargs->size());
        for (Oper::const_iterator it = cargs->begin(), itEnd = cargs->end(); it != itEnd; ++it) {
          args.push_back(String::cast(*it)->value());
        }
        if (!_process.begin(program->value(), args, _outputDir->value())) {
          // Tell manager we're done and in an error.
          _status = ERROR;
          _mgr->jobFinished(this);
        }
        return;
      }

      case Node::NK_ACTION_CLOSURE: {
        Oper * closure = static_cast<Oper *>(action);
        Node * callable = closure->arg(0);
        Oper * args = closure->arg(1)->asOper();
        Evaluator eval(_target->definition());
        eval.call(closure->location(), callable, _target->definition(), args->args());
        break;
      }

      default:
        diag::error(action->location()) << "Invalid action type: " << action;
        break;
    }
  }

  if (_status == ERROR) {
    _target->setState(Target::ERROR);

    // Remove any output files that got created.
    for (FileList::const_iterator
        it = _target->outputs().begin(), itEnd = _target->outputs().end(); it != itEnd; ++it) {
      File * outputFile = *it;
      outputFile->updateFileStatus();
      if (outputFile->exists()) {
        outputFile->remove();
      }
    }
  } else {
    _status = FINISHED;
    _target->setState(Target::FINISHED);

    // Ensure that all output files *actually* got created
    for (FileList::const_iterator
        it = _target->outputs().begin(), itEnd = _target->outputs().end(); it != itEnd; ++it) {
      File * outputFile = *it;
      outputFile->updateFileStatus();
      if (!outputFile->exists()) {
        Location loc = _target->location();
        if (_target->definition() != NULL && _target->definition()->name() != NULL) {
          Location loc = _target->definition()->name()->location();
        }
        diag::error(loc) << "Missing output file " << outputFile->name()
            << ", expected to be created by target " << _target;
      }
    }

    // For any dependent targets, see if they are ready.
    //diag::info() << "Checking dependent target states for target: " << _target;
    for (TargetList::const_iterator
        it = _target->dependents().begin(), itEnd = _target->dependents().end();
        it != itEnd; ++it) {
      Target * dep = *it;
      if (dep->state() == Target::WAITING) {
        dep->recheckState();
      }
      if (dep->state() == Target::READY) {
        _mgr->addReady(dep);
      }
    }
  }

  // Tell the manager we're done
  _mgr->jobFinished(this);
}

void Job::processFinished(Process & process, bool success) {
  if (!success) {
    _status = ERROR;
  }
  runNextAction();
}

void Job::trace() const {
  _mgr->mark();
  _target->mark();
  markArray(makeArrayRef(_actions.begin(), _actions.end()));
  safeMark(_outputDir);
}

// -------------------------------------------------------------------------
// JobMgr
// -------------------------------------------------------------------------

void JobMgr::addReady(Target * target) {
  if (target->state() == Target::INITIALIZED) {
    target->checkState();
  }

  switch (target->state()) {
    case Target::READY:
      target->setState(Target::READY_IN_QUEUE);
      _ready.push(target);
      break;

    case Target::READY_IN_QUEUE:
      break;

    case Target::WAITING: {
      // TODO: Add cycle check
      for (TargetList::const_iterator
          it = target->depends().begin(), itEnd = target->depends().end(); it != itEnd; ++it) {
        addReady(*it);
      }
      break;
    }
    case Target::FINISHED:
    case Target::BUILDING:
    case Target::ERROR:
      break;

    default:
      diag::error() << "Invalid state for target " << target << ": "
          << target->state();
      break;
  }
}

void JobMgr::addAllReady() {
  for (TargetMap::const_iterator it =
      _targets->targets().begin(), itEnd = _targets->targets().end(); it != itEnd; ++it) {
    String * path = it->second->path();
    if (path != NULL) {
      addReady(it->second);
    }
  }
}

Target * JobMgr::nextReady() {
  if (_ready.empty()) {
    return NULL;
  }
  Target * result = _ready.top();
  _ready.pop();
  return result;
}

void JobMgr::run() {
  for (;;) {
    while (_jobs.size() < _maxJobCount && !_error) {
      Target * target = nextReady();
      if (target != NULL) {
        //diag::status() << "Beginning target " << target << "\n";
        Job * job = new Job(this, target);
        _jobs.push_back(job);
        job->begin();
      } else if (_jobs.empty()) {
        // No ready targets and no jobs running
        //diag::info() << "No targets ready and no jobs running";
        return;
      } else {
        // No targets, but jobs are running, so wait for one.
        //diag::info() << "No targets, but jobs are still running";
        break;
      }
    }

    bool success = Process::waitForProcessEvent();
    if (!success) {
      _error = true;
    }
    if (_error && _jobs.empty()) {
      break;
    }
  }
}

void JobMgr::jobFinished(Job * job) {
  if (job->status() == Job::ERROR) {
    _error = true;
  }
  for (JobList::iterator it = _jobs.begin(), itEnd = _jobs.end(); it != itEnd; ++it) {
    if (*it == job) {
      _jobs.erase(it);
      break;
    }
  }
}

void JobMgr::trace() const {
  _targets->mark();
  markArray(ArrayRef<Job *>(_jobs));
}

}
