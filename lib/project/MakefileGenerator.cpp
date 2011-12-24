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

  // Certain special variables get translated directly into makefile variables.

  for (Attributes::const_iterator
      ai = _module->attrs().begin(), aiEnd = _module->attrs().end(); ai != aiEnd; ++ai) {
    Node * n = ai->second;
    Object * obj = n->asObject();
    if (obj->inheritsFrom(TypeRegistry::optionType()) ||
        obj->inheritsFrom(TypeRegistry::targetType())) {
      continue;
    }
  }

  // Set of all dependent files
  StringDict<char> depSet;

  //_mainModule->dump();
  for (TargetMap::const_iterator
      it = _targetMgr->targets().begin(), itEnd = _targetMgr->targets().end(); it != itEnd; ++it) {
    Target * target = it->second;
    if (target->path() != NULL && target->definition()->parentScope() == _module) {
      Object * targetObj = target->definition();
      Evaluator eval(targetObj);
      Oper * targetOutputs = eval.attributeValueAsList(targetObj, "outputs");
      for (Oper::const_iterator oi = targetOutputs->begin(), oiEnd = targetOutputs->end(); oi != oiEnd; ++oi) {
        String * outputFile = (*oi)->requireString(Location());
        _strm << outputFile->value() << " ";
      }
      _strm << ":";

      Oper * ideps = eval.attributeValueAsList(target->definition(), "implicit_depends");
      if (ideps == NULL) {
        continue;
      }
      for (Oper::const_iterator di = ideps->begin(), diEnd = ideps->end(); di != diEnd; ++di) {
        Object * dependentTarget = (*di)->requireObject();
        Evaluator evalDep(dependentTarget);
        Oper * outputs = evalDep.attributeValueAsList(dependentTarget, "outputs");
        for (Oper::const_iterator si = outputs->begin(), siEnd = outputs->end(); si != siEnd; ++si) {
          String * sourceFile = (*si)->requireString();
          _strm << " \\\n\t" << sourceFile->value();
          depSet[sourceFile] = '\0';
        }
      }

      Oper * actions = eval.attributeValueAsList(targetObj, "actions")->requireOper();
      if (actions != NULL) {
        for (Oper::const_iterator
            ai = actions->begin(), aiEnd = actions->end(); ai != aiEnd; ++ai) {
          writeAction((*ai)->requireOper((*ai)->location()), depSet);
        }
      }
      _strm << "\n\n";
    }
  }
}

void MakefileGenerator::writeAction(Oper * action, StringDict<char> & depSet) {
  if (action->nodeKind() == Node::NK_ACTION_COMMAND) {
    _strm << "\n\t" << action->arg(0);
    Oper * args = action->arg(1)->asOper();
    for (Oper::const_iterator
        ai = args->begin(), aiEnd = args->end(); ai != aiEnd; ++ai) {
      _strm << " " << *ai;
    }
  } else {
    action->dump();
  }
}

void MakefileGenerator::writeRelativePath(StringRef path) {
  SmallString<64> relPath;
  path::makeRelative(_module->buildDir(), path, relPath);
  _strm << relPath;
}

}
