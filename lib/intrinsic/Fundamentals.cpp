/* ================================================================== *
 * Fundamentals
 * ================================================================== */

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/FileSystem.h"

#include "mint/graph/GraphBuilder.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"

namespace mint {

Node * methodObjectPrototype(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return static_cast<Object *>(self)->prototype();
}

Node * methodObjectName(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return static_cast<Object *>(self)->name();
}

Node * methodTargetMake(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return NULL;
}

Node * methodToolRun(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return NULL;
}

Fundamentals::Fundamentals()
  : Module(Node::NK_MODULE, "<root>", NULL)
{
  Location loc;
  GraphBuilder builder(_typeRegistry);

  // Initialize all of the built-in types

  String * strName = str("name");
  String * strObject = str("object");
  String * strPrototype = str("prototype");
  String * strTarget = str("target");
  String * strTool = str("tool");

  Type * typeStringList = _typeRegistry.getListType(TypeRegistry::stringType());

  // Type 'object'
  object = new Object(loc, NULL);
  setProperty(strObject, object);
  object->setName(strObject);
  object->defineProperty(strPrototype,
      builder.createCall(loc, builder.createFunction(loc, object, methodObjectPrototype)),
      object);
  object->defineProperty(strName,
      builder.createCall(loc,
          builder.createFunction(loc, TypeRegistry::stringType(), methodObjectName)),
      TypeRegistry::stringType());

  // Type 'target'
  target = new Object(loc, object);
  setProperty(strTarget, target);
  target->setName(strTarget);

  // Create a type that is a list of files (strings?)
  Node * stringListEmpty = builder.createListOf(loc, TypeRegistry::stringType());
  target->defineProperty(str("sources"), stringListEmpty, typeStringList);
  target->defineProperty(str("outputs"), stringListEmpty, typeStringList);

  // Create a type that is a list of targets.
  Node * targetListEmpty = builder.createListOf(loc, target);
  target->defineProperty(str("depends"), targetListEmpty, targetListEmpty->type());

  // Type 'tool'
  tool = new Object(loc, object);
  setProperty(strTool, tool);
  tool->setName(strTool);

  defineBuilderProto();
  defineOptionProto();

  /// Function 'glob'.
  setProperty(
      str("glob"),
      builder.createFunction(loc, typeStringList, TypeRegistry::stringType(), fs::methodGlob));

  initConsole(this);
  initPath(this);
  initListType(this);
}

void Fundamentals::defineBuilderProto() {
  // Type 'builder'
  Type * typeStringList = _typeRegistry.getListType(TypeRegistry::stringType());
  builder = new Object(Location(), object);
  setProperty(str("builder"), builder);
  builder->setName(str("builder"));
  builder->defineProperty(str("output_types"), &Node::UNDEFINED_NODE, typeStringList);
  builder->defineProperty(str("sources"), &Node::UNDEFINED_NODE, typeStringList);
  builder->defineProperty(str("outputs"), &Node::UNDEFINED_NODE, typeStringList);
  builder->defineProperty(str("actions"), &Node::UNDEFINED_NODE, typeStringList, true);
  builder->defineProperty(str("target"), &Node::UNDEFINED_NODE, target);
}

void Fundamentals::defineOptionProto() {
  // Type 'option', which is *not* defined in the module's namespace, but is referred to
  // directly by the 'option' keyword.
  option = new Object(Location(), NULL);
  option->setName(str("option"));
  option->defineProperty(str("name"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
  option->defineProperty(str("help"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
  option->defineProperty(str("abbrev"), &Node::UNDEFINED_NODE, TypeRegistry::stringType());
}

String * Fundamentals::str(StringRef in) {
  StringDict<Node>::const_iterator it = _strings.find_as(in);
  if (it != _strings.end()) {
    return it->first;
  }
  String * result = String::create(Node::NK_IDENT, Location(), TypeRegistry::stringType(), in);
  _strings[result] = NULL;
  return result;
}

void Fundamentals::trace() const {
  Module::trace();
  _typeRegistry.trace();
  object->mark();
  target->mark();
  tool->mark();
  builder->mark();
  option->mark();
  list->mark();
  //dict->mark();
}

}
