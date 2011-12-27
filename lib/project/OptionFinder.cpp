/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/project/OptionFinder.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"

namespace mint {

OptionFinder::OptionFinder(Project * project)
  : _project(project)
{
  _optionProto = TypeRegistry::optionType();
}

void OptionFinder::visitObject(Object * obj) {
  if (obj->inheritsFrom(_optionProto)) {
#if 0
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
#endif
  }
}

}
