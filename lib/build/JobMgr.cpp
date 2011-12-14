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

  runNextAction();
}

void Job::runNextAction() {
  if (_actions.empty()) {
    return;
  }
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
      //action->dump();
      if (!_process.begin(program->value(), args, _outputDir->value())) {
        // Do what?
      }
      break;
    }

    default:
      diag::error(action->location()) << "Invalid action type: " << action;
      break;
  }
}

void Job::trace() const {
  _target->mark();
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
      //console::out() << "  " << path->value() << "\n";
      //it->second->checkState();
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
  while (_jobs.size() < _maxJobCount) {
    Target * target = nextReady();
    if (target == NULL) {
      if (_jobs.empty()) {
        // No ready targets and no jobs running
        return;
      }
      // Wait for something to do.
      return;
    }

    Job * job = new Job(target);
    _jobs.push_back(job);
    job->begin();
    //diag::status() << "Starting new job for: " << target << "\n";
  }
}

void JobMgr::trace() const {
  _targets->mark();
  markArray(ArrayRef<Job *>(_jobs));
}

}
