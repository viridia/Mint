/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/File.h"
#include "mint/build/Target.h"

#include "mint/graph/Object.h"

#include "mint/support/Diagnostics.h"

namespace mint {

String * Target::path() {
  return _definition->name();
}

Location Target::location() const {
  return _definition->location();
}

void Target::addDependency(Target * dep) {
  _depends.push_back(dep);
  dep->_dependents.push_back(this);
}

void Target::addSource(File * source) {
  _sources.push_back(source);
}

void Target::addOutput(File * output) {
  _outputs.push_back(output);
}

void Target::checkState() {
  if (_state == INITIALIZED) {
    _state = CHECKING_STATE;

    // Check output files
    bool needsRebuild = false;
    bool needsRebuildDeps = false;
    File * oldestOutput = NULL;
    for (FileList::const_iterator it = _outputs.begin(), itEnd = _outputs.end(); it != itEnd;
        ++it) {
      File * f = *it;
      if (!f->statusChecked()) {
        //console::out() << "    Checking status of " << f->name() << "\n";
        if (!f->updateFileStatus()) {
          continue;
        }
      }
      if (f->statusValid()) {
        if (!f->exists()) {
          if (!needsRebuild) {
            console::out() << "  Output " << f << " is missing.\n";
          }
          needsRebuild = true;
          break;
        } else if (oldestOutput == NULL || f->lastModified() < oldestOutput->lastModified()) {
          oldestOutput = f;
        }
      }
    }

    // Check source files. Don't bother checking source file timestamps if we know we need
    // a rebuild, or if there are no declared outputs.
    for (FileList::const_iterator it = _sources.begin(), itEnd = _sources.end(); it != itEnd;
        ++it) {
      File * f = *it;
      if (!f->statusChecked()) {
        //console::out() << "    Checking status of " << f->name() << "\n";
        if (!f->updateFileStatus()) {
          continue;
        }
      }
      if (f->statusValid()) {
        // If this file is the output of another target, then unless that target is up
        // to date, then it doesn't matter what the timestamp of the output is in.
        if (!f->exists()) {
          needsRebuild = true;
        }
        if (!f->outputOf().empty()) {
          for (TargetList::const_iterator ti = f->outputOf().begin(), tiEnd = f->outputOf().end();
              ti != tiEnd; ++ti) {
            Target * dep = *ti;
            if (dep->state() == CHECKING_STATE) {
              diag::error(this->location()) << "Circular dependency between target: " << this;
              diag::info(dep->location()) << "and target: " << dep;
              continue;
            }
            dep->checkState();
            if (dep->state() == READY || dep->state() == WAITING || dep->state() == BUILDING) {
              needsRebuild = true;
              needsRebuildDeps = true;
            }
            // TODO: Do we want to add to implicit deps?
          }
        } else if (!f->exists()) {
          diag::error(this->location()) << "Target " << this << " depends on non-existent file "
              << f->name();
          break;
        } else if (oldestOutput != NULL && oldestOutput->lastModified() < f->lastModified()) {
          if (!needsRebuild) {
            console::out() << "  Output " << oldestOutput << " is older than source " << f << ".\n";
          }
          needsRebuild = true;
        }
      }
    }

    for (TargetList::const_iterator ti = _depends.begin(), tiEnd = _depends.end(); ti != tiEnd;
        ++ti) {
      Target * dep = *ti;
      if (dep->state() == CHECKING_STATE) {
        diag::error(this->location()) << "Circular dependency between target: " << this;
        diag::info(dep->location()) << "and target: " << dep;
        continue;
      }
      dep->checkState();
      if (dep->state() == READY || dep->state() == WAITING || dep->state() == BUILDING) {
        needsRebuild = true;
        needsRebuildDeps = true;
      }
    }

    if (needsRebuild) {
      if (needsRebuildDeps) {
        _state = WAITING;
        console::out() << "Target " << this << " is waiting on other targets\n";
      } else {
        _state = READY;
        console::out() << "Target " << this << " is ready to build\n";
      }
    } else {
      _state = FINISHED;
      console::out() << "Target " << this << " is up to date\n";
      console::out() << "Target has " << _depends.size() << " dependencies, and " << _dependents.size() << " dependents.\n";
    }
  } else {
    //console::out() << "Target " << this << " is in a weird state: " << _state << ".\n";
  }
}

void Target::print(OStream & strm) const {
  if (_definition->name()) {
    strm << _definition->name();
  } else if (!_sources.empty()) {
    strm << _sources[0];
  }
}

void Target::trace() const {
  _definition->mark();
  markArray(ArrayRef<Target *>(_depends));
  markArray(ArrayRef<Target *>(_dependents));
  markArray(ArrayRef<File *>(_sources));
  markArray(ArrayRef<File *>(_outputs));
}

OStream & operator<<(OStream & strm, const Target & target) {
  target.print(strm);
  return strm;
}

OStream & operator<<(OStream & strm, const Target * target) {
  target->print(strm);
  return strm;
}

OStream & operator<<(OStream & strm, Target::TargetState state) {
  switch (state) {
    case Target::UNINIT:
      strm << "UNINIT";
      break;

    case Target::INITIALIZING:
      strm << "INITIALIZING";
      break;

    case Target::INITIALIZED:
      strm << "INITIALIZED";
      break;

    case Target::CHECKING_STATE:
      strm << "CHECKING_STATE";
      break;

    case Target::WAITING:
      strm << "WAITING";
      break;

    case Target::READY:
      strm << "READY";
      break;

    case Target::BUILDING:
      strm << "BUILDING";
      break;

    case Target::FINISHED:
      strm << "FINISHED";
      break;

    case Target::ERROR:
      strm << "ERROR";
      break;
  }
  return strm;
}

}
