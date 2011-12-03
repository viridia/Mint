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
  setProperty(str("fundamentals"), this);

  // Initialize all of the built-in types

  defineObjectProto();
  defineTargetProto();
  defineOptionProto();
  initModuleType();

  // Built-in methods that are in specific namespaces

  initConsoleMethods(this);
  initPathMethods(this);
  initRegExMethods(this);

  // Built-in methods that are in the global namespace

  initSubprocessMethods(this);
  initFileMethods(this);
  initDirSearchMethods(this);

  // Built-in methods that are associated with a particular type.

  initListMethods(this);

  // Built-in global methods

  object->defineMethod("require", TypeRegistry::anyType(), TypeRegistry::anyType(), functionRequire);
}

void Fundamentals::defineObjectProto() {
  // Type 'object'
  object = createChildObject("object");
  object->defineDynamicAttribute("prototype", object, methodObjectPrototype);
  object->defineDynamicAttribute("name", TypeRegistry::stringType(), methodObjectName);
  object->defineDynamicAttribute("module", TypeRegistry::moduleType(), methodObjectModule);
  object->defineMethod("parent", TypeRegistry::stringType(), methodObjectParent);
}

void Fundamentals::defineTargetProto() {
  GraphBuilder builder;

  // Type 'target'
  target = createChildObject("target", object);

  // Create a type that is a list of files (strings?)
  Type * typeStringList = TypeRegistry::get().getListType(TypeRegistry::stringType());
  Node * stringListEmpty = builder.createListOf(Location(), TypeRegistry::stringType());
  target->defineAttribute(str("sources"), stringListEmpty, typeStringList);
  target->defineAttribute(str("outputs"), stringListEmpty, typeStringList,
      AttributeDefinition::LAZY | AttributeDefinition::EXPORT);

  // Create a type that is a list of targets.
  Node * targetListEmpty = builder.createListOf(Location(), target);
  target->defineAttribute(str("depends"), targetListEmpty, targetListEmpty->type());
}

void Fundamentals::defineOptionProto() {
  // Type 'option', which is *not* defined in the module's namespace, but is referred to
  // directly by the 'option' keyword.
  option = new Object(Location(), NULL);
  option->setName("option");
  option->defineAttribute(str("name"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
  option->defineAttribute(str("help"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
  option->defineAttribute(str("abbrev"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
}

String * Fundamentals::str(StringRef in) {
  return StringRegistry::str(in);
}

Object * Fundamentals::createChildObject(StringRef name, Object * prototype) {
  String * strName = str(name);
  Object * obj = new Object(Location(), prototype);
  obj->setName(strName);
  setProperty(strName, obj);
  return obj;
}

Object * Fundamentals::createChildScope(StringRef name) {
  String * strName = str(name);
  Object * obj = new Object(Node::NK_DICT, Location(), NULL);
  obj->setName(strName);
  setProperty(strName, obj);
  return obj;
}

Fundamentals & Fundamentals::get() {
  static GCPointerRoot<Fundamentals> instance = new Fundamentals();
  return *instance;
}

void Fundamentals::trace() const {
  Module::trace();
  object->mark();
  target->mark();
  option->mark();
  list->mark();
  //dict->mark();
}

}
