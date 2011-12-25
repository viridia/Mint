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
  _strm << "# -----------------------------------------------------------------------------\n";
  _strm << "# Makefile for main module\n";
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

  SmallVector<String *, 16> allTargets;
  for (TargetMap::const_iterator
      it = _targetMgr->targets().begin(), itEnd = _targetMgr->targets().end(); it != itEnd; ++it) {
    Target * target = it->second;
    if (target->definition()->module() == _module) {
      for (FileList::const_iterator fi = target->outputs().begin(), fiEnd = target->outputs().end();
          fi != fiEnd; ++fi) {
        allTargets.push_back((*fi)->name());
      }
    }
  }

  _strm << "all:";
  SmallString<64> relPath;
  for (SmallVectorImpl<String *>::const_iterator it = allTargets.begin(), itEnd = allTargets.end(); it != itEnd; ++it) {
    // TODO: Quoting
    makeRelative((*it)->value(), relPath);
    _strm << " " << relPath;
  }
  _strm << "\n\n";

  for (TargetMap::const_iterator
      it = _targetMgr->targets().begin(), itEnd = _targetMgr->targets().end(); it != itEnd; ++it) {
    Target * target = it->second;
    writeTarget(target);
  }

  if (!_outputs.empty()) {
    _strm << "clean:\n";
    _strm << "\t@rm -rf ";
    for (SmallVectorImpl<String *>::const_iterator it = _outputs.begin(), itEnd = _outputs.end(); it != itEnd; ++it) {
      makeRelative((*it)->value(), relPath);
      _strm << " \\\n\t" << relPath;
    }
    _strm << "\n\n";
  }
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
  for (FileList::const_iterator fi = target->sources().begin(), fiEnd = target->sources().end();
      fi != fiEnd; ++fi) {
    inputFiles.push_back((*fi)->name());
  }
  for (TargetList::const_iterator ti = target->depends().begin(), tiEnd = target->depends().end();
      ti != tiEnd; ++ti) {
    Target * dep = *ti;
    for (FileList::const_iterator fi = dep->outputs().begin(), fiEnd = dep->outputs().end();
        fi != fiEnd; ++fi) {
      inputFiles.push_back((*fi)->name());
    }
  }

  // Write out the list of output files.
  StringDict<char> depFiles;
  SmallString<64> relPath;
  for (FileList::const_iterator fi = target->outputs().begin(), fiEnd = target->outputs().end();
      fi != fiEnd; ++fi) {
    makeRelative((*fi)->name()->value(), relPath);
    _outputs.push_back((*fi)->name());
    _strm << relPath << " ";
  }
  _strm << ":";

  // Write out the list of input files
  for (SmallVectorImpl<String *>::const_iterator
      si = inputFiles.begin(), siEnd = inputFiles.end(); si != siEnd; ++si) {
    makeRelative((*si)->value(), relPath);
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

}
