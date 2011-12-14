/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/JobMgr.h"

#include "mint/eval/Evaluator.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"

namespace mint {

void Job::begin() {
  Evaluator eval(_target->definition());
  Object * targetObj = _target->definition();
  Node * actions = eval.attributeValue(targetObj, "actions");
  Node * outputDir = eval.attributeValue(targetObj, "output_dir");
  M_ASSERT(actions != NULL);
  M_ASSERT(outputDir != NULL);
  actions->dump();
  outputDir->dump();
}

void Job::trace() const {
  _target->mark();
}

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
    diag::status() << "Starting new job for: " << target << "\n";
  }
}

void JobMgr::trace() const {
  _targets->mark();
  markArray(ArrayRef<Job *>(_jobs));
}

}
