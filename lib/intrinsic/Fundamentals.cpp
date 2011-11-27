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

  // Listed in it's own namespace
  setProperty(str("fundamentals"), this);

  // Initialize all of the built-in types

  Type * typeStringList = _typeRegistry.getListType(TypeRegistry::stringType());

  defineObjectProto();
  defineTargetProto();
  defineOptionProto();

  /// Function 'glob'.
  setProperty(
      str("glob"),
      builder.createFunction(loc, typeStringList, TypeRegistry::stringType(), fs::methodGlob));

  initConsole(this);
  initPath(this);
  initListType(this);
  initSubprocess(this);
}

void Fundamentals::defineObjectProto() {
  GraphBuilder builder(_typeRegistry);

  // Type 'object'
  object = new Object(Location(), NULL);
  setProperty(str("object"), object);
  object->setName(str("object"));
  object->defineProperty(str("prototype"),
      builder.createCall(Location(),
          builder.createFunction(Location(), object, methodObjectPrototype)),
      object);
  object->defineProperty(str("name"),
      builder.createCall(
          Location(),
          builder.createFunction(Location(), TypeRegistry::stringType(), methodObjectName)),
          TypeRegistry::stringType());
}

void Fundamentals::defineTargetProto() {
  GraphBuilder builder(_typeRegistry);

  // Type 'target'
  target = new Object(Location(), object);
  setProperty(str("target"), target);
  target->setName(str("target"));

  // Create a type that is a list of files (strings?)
  Type * typeStringList = _typeRegistry.getListType(TypeRegistry::stringType());
  Node * stringListEmpty = builder.createListOf(Location(), TypeRegistry::stringType());
  target->defineProperty(str("sources"), stringListEmpty, typeStringList);
  target->defineProperty(str("outputs"), stringListEmpty, typeStringList, true);

  // Create a type that is a list of targets.
  Node * targetListEmpty = builder.createListOf(Location(), target);
  target->defineProperty(str("depends"), targetListEmpty, targetListEmpty->type());
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
  option->mark();
  list->mark();
  //dict->mark();
}

}
