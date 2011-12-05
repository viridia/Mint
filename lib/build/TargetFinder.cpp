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
      Node * depends = obj->getAttributeValue("depends");
      Node * implicit_depends = obj->getAttributeValue("implicit_depends");
      Node * sources = eval.attributeValue(obj, "sources");
      Node * outputs = eval.attributeValue(obj, "outputs");
      Node * source_dir = eval.attributeValue(obj, "source_dir");
      Node * output_dir = eval.attributeValue(obj, "output_dir");

      // Default source directory
      StringRef sourceDir = module->sourceDir();
      if (source_dir != NULL && source_dir->nodeKind() == Node::NK_STRING) {
        sourceDir = static_cast<String *>(source_dir)->value();
      }

      // Default output directory
      StringRef outputDir = module->buildDir();
      if (output_dir != NULL && output_dir->nodeKind() == Node::NK_STRING) {
        outputDir = static_cast<String *>(output_dir)->value();
      }

      // Explicit dependencies
      M_ASSERT(depends != NULL);
      M_ASSERT(depends->nodeKind() == Node::NK_LIST) << "Invalid depends: " << depends;
      addDependenciesToTarget(target, static_cast<Oper *>(depends));

      // Implicit dependencies
      M_ASSERT(implicit_depends != NULL);
      M_ASSERT(implicit_depends->nodeKind() == Node::NK_LIST) << "Invalid implicit_depends: "
          << implicit_depends;
      addDependenciesToTarget(target, static_cast<Oper *>(implicit_depends));

      // Explicit sources
      M_ASSERT(sources != NULL);
      M_ASSERT(sources->nodeKind() == Node::NK_LIST) << "Invalid sources: " << sources;
      addSourcesToTarget(target, static_cast<Oper *>(sources), sourceDir);

      // Outputs
      M_ASSERT(outputs != NULL);
      M_ASSERT(outputs->nodeKind() == Node::NK_LIST) << "Invalid outputs: " << outputs;
      addOutputsToTarget(target, static_cast<Oper *>(outputs), outputDir);

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
        target->addDependency(depTarget);
        visit(dep);
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
    if (n->nodeKind() != Node::NK_STRING) {
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
