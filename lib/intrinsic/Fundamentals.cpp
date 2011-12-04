/* ================================================================== *
 * Fundamentals
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"

#include "mint/graph/GraphBuilder.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"

namespace mint {

Node * methodObjectPrototype(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return static_cast<Object *>(self)->prototype();
}

Node * methodObjectName(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return static_cast<Object *>(self)->name();
}

Node * methodObjectParent(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return self->parentScope();
}

Node * methodObjectModule(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  for (Node * n = self; n != NULL; n = n->parentScope()) {
    if (n->nodeKind() == Node::NK_MODULE) {
      return n;
    }
  }
  return &Node::UNDEFINED_NODE;
}

Node * functionRequire(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  if (!ex->isNonNil(args[0])) {
    diag::error(loc) << "Missing required value";
  }
  return &Node::UNDEFINED_NODE;
}

Fundamentals::Fundamentals()
  : Module(Node::NK_MODULE, "<root>", NULL)
{
  Location loc;

  // Listed in it's own namespace
  setAttribute(str("fundamentals"), this);

  // Initialize all of the built-in types

  initObjectType();
  initTargetType();
  initOptionType();
  initModuleType();
  initListType();

  // Built-in methods that are in specific namespaces

  initConsoleMethods(this);
  initPathMethods(this);
  initRegExMethods(this);

  // Built-in methods that are in the global namespace

  initSubprocessMethods(this);
  initFileMethods(this);
  initDirSearchMethods(this);

  // Built-in global methods

}

void Fundamentals::initObjectType() {
  // Type 'object'
  Object * objectType = TypeRegistry::objectType();
  setAttribute(objectType->name(), objectType);
  if (objectType->attrs().empty()) {
    objectType->defineDynamicAttribute("prototype", objectType, methodObjectPrototype);
    objectType->defineDynamicAttribute("name", TypeRegistry::stringType(), methodObjectName);
    objectType->defineDynamicAttribute("module", TypeRegistry::moduleType(), methodObjectModule);
    objectType->defineMethod("parent", TypeRegistry::stringType(), methodObjectParent);
    objectType->defineMethod("require", TypeRegistry::anyType(), TypeRegistry::anyType(),
        functionRequire);
  }
}

void Fundamentals::initTargetType() {
  GraphBuilder builder;

  // Type 'target'
  Object * targetType = TypeRegistry::targetType();
  setAttribute(targetType->name(), targetType);
  if (targetType->attrs().empty()) {
    targetType->setType(TypeRegistry::objectType());

    // Create a type that is a list of files (strings?)
    Type * typeStringList = TypeRegistry::get().getListType(TypeRegistry::stringType());
    Node * stringListEmpty = builder.createListOf(Location(), TypeRegistry::stringType());
    targetType->defineAttribute(str("sources"), stringListEmpty, typeStringList);
    targetType->defineAttribute(str("outputs"), stringListEmpty, typeStringList,
        AttributeDefinition::EXPORT);

    // Create a type that is a list of targets.
    Node * targetListEmpty = builder.createListOf(Location(), targetType);
    targetType->defineAttribute(str("depends"), targetListEmpty, targetListEmpty->type());
  }
}

void Fundamentals::initOptionType() {
  // Type 'option', which is *not* defined in the module's namespace, but is referred to
  // directly by the 'option' keyword.
  Object * optionType = TypeRegistry::optionType();
  if (optionType->attrs().empty()) {
    optionType->setName("option");
    optionType->defineAttribute(str("name"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
    optionType->defineAttribute(str("help"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
    optionType->defineAttribute(str("abbrev"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
  }
}

String * Fundamentals::str(StringRef in) {
  return StringRegistry::str(in);
}

Object * Fundamentals::createChildScope(StringRef name) {
  String * strName = str(name);
  Object * obj = new Object(Node::NK_DICT, Location(), NULL);
  obj->setName(strName);
  setAttribute(strName, obj);
  return obj;
}

Fundamentals & Fundamentals::get() {
  static GCPointerRoot<Fundamentals> instance(new Fundamentals());
  return *instance;
}

void Fundamentals::trace() const {
  Module::trace();
}

}
