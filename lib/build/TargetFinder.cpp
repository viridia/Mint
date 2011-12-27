/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/Target.h"
#include "mint/build/TargetMgr.h"
#include "mint/build/TargetFinder.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"

namespace mint {

TargetFinder::TargetFinder(TargetMgr * targetMgr, Project * project)
  : _targetMgr(targetMgr)
  , _project(project)
{
  _targetProto = TypeRegistry::targetType();
}

void TargetFinder::visitObject(Object * obj) {
  if (obj->inheritsFrom(_targetProto)) {
    Target * target = _targetMgr->getTarget(obj);
    if (target->state() == Target::UNINIT) {
      target->setState(Target::INITIALIZING);
      Module * module = obj->module();
      M_ASSERT(module != NULL);

      Evaluator eval(module);
      Oper * depends = eval.attributeValueAsList(obj, "depends");
      Oper * implicit_depends = eval.attributeValueAsList(obj, "implicit_depends");
      Oper * sources = eval.attributeValueAsList(obj, "sources");
      Oper * outputs = eval.attributeValueAsList(obj, "outputs");
      Node * source_dir = eval.attributeValue(obj, "source_dir");
      Node * output_dir = eval.attributeValue(obj, "output_dir");

      if (eval.attributeValueAsBool(obj, "exclude_from_all")) {
        target->setFlag(Target::EXCLUDE_FROM_ALL, true);
      }
      if (eval.attributeValueAsBool(obj, "source_only")) {
        target->setFlag(Target::SOURCE_ONLY, true);
      }
      if (eval.attributeValueAsBool(obj, "internal")) {
        target->setFlag(Target::INTERNAL, true);
      }

      // Default source directory
      StringRef sourceDir = module->sourceDir();
      if (source_dir != NULL && source_dir->nodeKind() == Node::NK_STRING) {
        sourceDir = static_cast<String *>(source_dir)->value();
        M_ASSERT(path::isAbsolute(sourceDir));
      }

      // Default output directory
      StringRef outputDir = module->buildDir();
      if (output_dir != NULL && output_dir->nodeKind() == Node::NK_STRING) {
        outputDir = static_cast<String *>(output_dir)->value();
        M_ASSERT(path::isAbsolute(outputDir));
      }

      // Explicit dependencies
      M_ASSERT(depends != NULL);
      addDependenciesToTarget(target, depends);

      // Implicit dependencies
      M_ASSERT(implicit_depends != NULL);
      addDependenciesToTarget(target, implicit_depends);

      // Explicit sources - but only if this is not a delegating builder
      if (implicit_depends->size() == 0) {
        M_ASSERT(sources != NULL);
        addSourcesToTarget(target, sources, sourceDir);
      }

      // Outputs
      M_ASSERT(outputs != NULL);
      addOutputsToTarget(target, outputs, outputDir);

      target->setState(Target::INITIALIZED);
    }
  }
}

void TargetFinder::addDependenciesToTarget(Target * target, Oper * list) {
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = *it;
    if (n->nodeKind() != Node::NK_OBJECT) {
      diag::error(n->location()) << "Invalid type for target dependency: " << n->nodeKind();
      diag::info(target->definition()->location()) << "For target: " << target->definition();
    } else {
      Object * dep = static_cast<Object *>(n);
      if (!dep->inheritsFrom(_targetProto)) {
        diag::error(n->location()) << "Invalid type for target dependency: " << n->nodeKind();
        diag::info(target->definition()->location()) << "For target: " << target->definition();
      } else {
        Target * depTarget = _targetMgr->getTarget(dep);
        if (depTarget != target) {
          target->addDependency(depTarget);
          visit(dep);
        }
      }
    }
  }
}

void TargetFinder::addSourcesToTarget(Target * target, Oper * list, StringRef baseDir) {
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = *it;
    if (n->nodeKind() != Node::NK_STRING) {
      diag::error(n->location()) << "Invalid type for source file: " << n->nodeKind();
      diag::info(target->definition()->location()) << "For target: " << target->definition();
    } else {
      String * path = makeAbsolute(static_cast<String *>(n), baseDir);
      File * file = _targetMgr->getFile(path);
      target->addSource(file);
      file->addSourceFor(target);
    }
  }
}

void TargetFinder::addOutputsToTarget(Target * target, Oper * list, StringRef baseDir) {
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = *it;
    if (n->nodeKind() != Node::NK_STRING && n->nodeKind() != Node::NK_IDENT) {
      diag::error(n->location()) << "Invalid type for output file: " << n->nodeKind();
      diag::info(target->definition()->location()) << "For target: " << target->definition();
    } else {
      String * path = makeAbsolute(static_cast<String *>(n), baseDir);
      File * file = _targetMgr->getFile(path);
      target->addOutput(file);
      file->addOutputOf(target);
    }
  }
}

String * TargetFinder::makeAbsolute(String * filepath, StringRef baseDir) {
  if (path::isAbsolute(filepath->value())) {
    return filepath;
  }
  SmallString<128> absPath(baseDir);
  path::combine(absPath, filepath->value());
  return String::create(filepath->location(), absPath);
}

}
