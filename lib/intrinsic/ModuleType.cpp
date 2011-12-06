/* ================================================================== *
 * Fundamentals
 * ================================================================== */

#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"

namespace mint {

Node * methodModuleSourceDir(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(self->nodeKind() == Node::NK_MODULE);
  Module * m = static_cast<Module *>(self);
  return String::create(m->sourceDir());
}

Node * methodModuleBuildDir(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(self->nodeKind() == Node::NK_MODULE);
  Module * m = static_cast<Module *>(self);
  return String::create(m->buildDir());
}

void initModuleType() {
  Object * moduleType = TypeRegistry::moduleType();
  if (moduleType->attrs().empty()) {
    moduleType->setType(TypeRegistry::objectType());
    moduleType->defineDynamicAttribute(
        "source_dir", TypeRegistry::stringType(), methodModuleSourceDir);
    moduleType->defineDynamicAttribute(
        "output_dir", TypeRegistry::stringType(), methodModuleBuildDir);
  }
}

}
