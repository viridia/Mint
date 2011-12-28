/* ================================================================== *
 * Mint: A refreshing approach to build configuration.
 * ================================================================== */

#include "mint/build/Target.h"
#include "mint/build/TargetMgr.h"

#include "mint/eval/Evaluator.h"

#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/Type.h"
#include "mint/graph/String.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/MakefileGenerator.h"
#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"

namespace mint {

void MakefileGenerator::writeModule() {
  SmallString<64> relPath;

  _strm << "# -----------------------------------------------------------------------------\n";
  if (_module == _module->project()->mainModule()) {
    _strm << "# Makefile for main module\n";
  } else {
    SmallString<64> mPath;
    path::makeRelative(_module->project()->mainModule()->buildDir(), _module->buildDir(), mPath);
    _strm << "# Makefile for " << mPath << "\n";
  }
  _strm << "# -----------------------------------------------------------------------------\n\n";
  _strm << "VPATH = ";
  writeRelativePath(_module->sourceDir());
  _strm << "\n\n";
  _strm << ".PHONY: all clean\n\n";

  for (Attributes::const_iterator
      ai = _module->attrs().begin(), aiEnd = _module->attrs().end(); ai != aiEnd; ++ai) {
    Node * n = ai->second;
    Object * obj = n->asObject();
    if (obj->inheritsFrom(TypeRegistry::optionType()) ||
        obj->inheritsFrom(TypeRegistry::targetType())) {
      continue;
    }
  }

  // Collect targets
  SmallVector<String *, 16> allTargets;
  TargetList moduleTargets; // Targets defined in this module
  TargetMap externalTargets; // Targets defined in this module
  for (TargetMap::const_iterator
      it = _targetMgr->targets().begin(), itEnd = _targetMgr->targets().end(); it != itEnd; ++it) {
    Target * target = it->second;
    // Don't write out the target if it's merely a collection of files.
    if (target->isSourceOnly()) {
      continue;
    }
    Object * targetObj = target->definition();
    if (targetObj->module() == _module) {
      if (target->path() != NULL) {
        _uniqueNames[target->path()] = NULL;
      }

      if (!target->isExcludeFromAll()) {
        // Collect files for the 'all' target
        for (FileList::const_iterator
            fi = target->outputs().begin(), fiEnd = target->outputs().end(); fi != fiEnd; ++fi) {
          allTargets.push_back((*fi)->name());
        }
        if (target->outputs().empty() && target->path() != NULL) {
          allTargets.push_back(target->path());
        }
      }

      moduleTargets.push_back(target);
      for (TargetList::const_iterator
          ti = target->depends().begin(), tiEnd = target->depends().end(); ti != tiEnd; ++ti) {
        Target * dep = *ti;
        if (dep->path() == NULL) {
          // If the target has no path, then it's only reachable from this module.
          moduleTargets.push_back(dep);
        } else if (dep->definition()->module() != _module) {
          externalTargets[dep->definition()] = dep;
        }
      }
    }
  }

  // Make the 'all' target
  if (!allTargets.empty()) {
    _strm << "all:";
    std::sort(allTargets.begin(), allTargets.end(), StringComparator());
    for (SmallVectorImpl<String *>::const_iterator
        it = allTargets.begin(), itEnd = allTargets.end(); it != itEnd; ++it) {
      // TODO: Quoting
      makeRelative((*it)->value(), relPath);
      _strm << " " << relPath;
    }
    _strm << "\n\n";
  }

  std::sort(moduleTargets.begin(), moduleTargets.end(), TargetComparator());
  for (TargetList::const_iterator
      it = moduleTargets.begin(), itEnd = moduleTargets.end(); it != itEnd; ++it) {
    writeTarget(*it);
  }

  for (TargetMap::const_iterator
      it = externalTargets.begin(), itEnd = externalTargets.end(); it != itEnd; ++it) {
    writeExternalTarget(it->second);
  }

  // Generate the 'clean' target
  if (!_cleanFiles.empty() || !externalTargets.empty()) {
    _strm << "clean:\n";
    for (TargetMap::const_iterator
        it = externalTargets.begin(), itEnd = externalTargets.end(); it != itEnd; ++it) {
      Target * ext = it->second;
      _strm << "\t${MAKE} -C " << ext->definition()->module()->buildDir() << " clean\n";
    }
    _strm << "\t@rm -rf";
    for (SmallVectorImpl<String *>::const_iterator
        it = _cleanFiles.begin(), itEnd = _cleanFiles.end(); it != itEnd; ++it) {
      makeOutputRelative((*it)->value(), relPath);
      _strm << " \\\n\t" << relPath;
    }
    _strm << "\n\n";
  }

  path::writeFileContentsIfDifferent(_outputPath, _strm.str());
}

void MakefileGenerator::writeTarget(Target * target) {
  Object * targetObj = target->definition();

  // Temporarily change the 'implicit_sources' variable to the makefile syntax for the
  // dependent files in this makefile.
  targetObj->attrs()[String::create("implicit_sources")] =
      Oper::createList(
          Location(), TypeRegistry::stringListType(), String::create(Location(), "$^"));

  // List of all input files to the command
  // This consists of the outputs of all dependencies and implicit dependencies.
  SmallVector<String *, 16> inputFiles;
  if (!target->isSourceOnly()) {
    for (FileList::const_iterator fi = target->sources().begin(), fiEnd = target->sources().end();
        fi != fiEnd; ++fi) {
      String * filename = (*fi)->name();
      inputFiles.push_back(filename);
    }
  }
  for (TargetList::const_iterator ti = target->depends().begin(), tiEnd = target->depends().end();
      ti != tiEnd; ++ti) {
    Target * dep = *ti;
    for (FileList::const_iterator fi = dep->outputs().begin(), fiEnd = dep->outputs().end();
        fi != fiEnd; ++fi) {
      String * filename = (*fi)->name();
      inputFiles.push_back(filename);
    }
  }

  if (target->outputs().empty() && target->path() != NULL) {
    _strm << ".PHONY: " << target->path()->value() << "\n\n";
    _strm << target->path()->value() << " ";
  }

  // Write out the list of output files.
  StringDict<char> depFiles;
  SmallString<64> relPath;
  for (FileList::const_iterator fi = target->outputs().begin(), fiEnd = target->outputs().end();
      fi != fiEnd; ++fi) {
    String * filename = (*fi)->name();
    if (filename->value().startsWith(_module->buildDir())) {
      // Only add to the list of targets to be cleaned if it's within the build directory.
      _cleanFiles.push_back(filename);
    }
    makeRelative(filename->value(), relPath);
    _strm << relPath << " ";
  }

  _strm << ":";

  // Write out the list of input files
  for (SmallVectorImpl<String *>::const_iterator
      si = inputFiles.begin(), siEnd = inputFiles.end(); si != siEnd; ++si) {
    makeOutputRelative((*si)->value(), relPath);
    _strm << " \\\n\t" << relPath;
  }

  // Write out the actions
  SmallVector<String *, 16> inputArgs;
  Evaluator eval(targetObj);
  Oper * actions = eval.attributeValueAsList(targetObj, "actions");
  if (actions != NULL) {
    for (Oper::const_iterator
        ai = actions->begin(), aiEnd = actions->end(); ai != aiEnd; ++ai) {
      writeAction((*ai)->requireOper((*ai)->location()));
    }
  }
  _strm << "\n\n";
}

void MakefileGenerator::writeExternalTarget(Target * target) {
  Object * targetObj = target->definition();
  Module * targetModule = targetObj->module();

  // Write out the list of output files.
  StringDict<char> depFiles;
  SmallString<64> relPath;
  for (FileList::const_iterator fi = target->outputs().begin(), fiEnd = target->outputs().end();
      fi != fiEnd; ++fi) {
    makeRelative((*fi)->name()->value(), relPath);
    _strm << relPath << " ";
  }
  _strm << ":\n";
  _strm << "\t$(MAKE) -C " << targetModule->buildDir();
  for (FileList::const_iterator fi = target->outputs().begin(), fiEnd = target->outputs().end();
      fi != fiEnd; ++fi) {
    SmallString<64> relPath;
    path::makeRelative(targetModule->buildDir(), (*fi)->name()->value(), relPath);
    _strm << " " << relPath;
  }
  _strm << "\n\n";
}

void MakefileGenerator::writeAction(Oper * action) {
  if (action->nodeKind() == Node::NK_ACTION_COMMAND) {
    _strm << "\n\t@" << action->arg(0)->requireString()->value();
    Oper * args = action->arg(1)->requireOper();
    for (Oper::const_iterator
        ai = args->begin(), aiEnd = args->end(); ai != aiEnd; ++ai) {
      // TODO: Quote if needed
      _strm << " " << (*ai)->requireString()->value();
    }
  } else if (action->nodeKind() == Node::NK_ACTION_MESSAGE) {
    diag::Severity severity = diag::Severity(action->arg(0)->requireInt());
    (void)severity;
    StringRef msg = action->arg(1)->requireString()->value();
    if (msg[msg.size() - 1] == '\n') {
      msg = msg.substr(0, msg.size() - 1);
    }
    // TODO: Quoting
    // TODO: ANSI colors?
    _strm << "\n\t@echo \"" << msg << "\"";
  } else {
    action->dump();
    M_ASSERT(false) << "Unsupported action type for makefile generator!";
  }
}

void MakefileGenerator::writeRelativePath(StringRef path) {
  SmallString<64> relPath;
  path::makeRelative(_module->buildDir(), path, relPath);
  _strm << relPath;
}

void MakefileGenerator::makeRelative(StringRef inPath, SmallVectorImpl<char> & result) {
  if (inPath.startsWith(_module->buildDir())) {
    path::makeRelative(_module->buildDir(), inPath, result);
  } else if (inPath.startsWith(_module->sourceDir())) {
    path::makeRelative(_module->sourceDir(), inPath, result);
  } else {
    result.assign(inPath.begin(), inPath.end());
  }
}

void MakefileGenerator::makeOutputRelative(StringRef inPath, SmallVectorImpl<char> & result) {
  if (inPath.startsWith(_module->buildDir())) {
    path::makeRelative(_module->buildDir(), inPath, result);
  } else if (inPath.startsWith(_module->sourceDir())) {
    path::makeRelative(_module->buildDir(), inPath, result);
  } else {
    result.assign(inPath.begin(), inPath.end());
  }
}

String * MakefileGenerator::uniqueName(String * stem) {
  if (_uniqueNames.find(stem) == _uniqueNames.end()) {
    return stem;
  }
  SmallString<32> newName;
  for (unsigned counter = 1;; ++counter) {
    OStrStream strm;
    strm << stem << "_" << counter;
    newName = stem->value();
    if (_uniqueNames.find_as(strm.str()) == _uniqueNames.end()) {
      String * result = String::create(stem->location(), strm.str());
      _uniqueNames[result] = NULL;
      return result;
    }
  }
}

}
