/* ================================================================== *
 * Fundamentals
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"

#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"

#include "mint/intrinsic/TypeRegistry.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"

namespace mint {

// -------------------------------------------------------------------------
// Global functions
// -------------------------------------------------------------------------

Node * functionRequire(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  if (!ex->isNonNil(args[0])) {
    diag::error(loc) << "Missing required value";
  }
  return &Node::UNDEFINED_NODE;
}

Node * functionCommand(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  M_ASSERT(args[0]->nodeKind() == Node::NK_STRING);
  M_ASSERT(args[1]->nodeKind() == Node::NK_LIST);
  return Oper::create(Node::NK_ACTION_COMMAND, loc, TypeRegistry::actionType(), args);
}

Node * functionMakeArglist(Location loc, Evaluator * ex, Function * fn, Node * self,
    NodeArray args) {
  M_ASSERT(args.size() == 1);
  Oper * list = args[0]->requireOper();
  SmallVector<Node *, 16> result;
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = *it;
    if (ex->isNonNil(n)) {
      Node * s = ex->coerce(loc, n, TypeRegistry::stringType());
      if (s != NULL) {
        result.push_back(s);
      }
    }
  }
  return Oper::create(Node::NK_LIST, loc, TypeRegistry::stringListType(), result);
}

Fundamentals::Fundamentals() : Module("<fundamentals>", NULL) {
  Location loc;

  // Listed in it's own namespace
  setAttribute(str("fundamentals"), this);

  // Initialize all of the built-in types

  initObjectType();
  initTargetType();
  initOptionType();
  initModuleType();
  initActionType();
  initStringType();
  initListType();

  // Built-in namespaces

  initPlatformVars();

  // Built-in methods that are in specific namespaces

  initConsoleMethods(this);
  initPathMethods(this);
  initRegExMethods(this);

  // Built-in methods that are in the global namespace

  initSubprocessMethods(this);
  initFileMethods(this);
  initDirSearchMethods(this);

  // Built-in global methods
  defineMethod("require", TypeRegistry::anyType(), TypeRegistry::anyType(), functionRequire);
  defineMethod("command", "A,program:s,args:[s", functionCommand);
  defineMethod("make_arglist", "[s,args:*a", functionMakeArglist);
}

// -------------------------------------------------------------------------
// Object
// -------------------------------------------------------------------------

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
  Node * module = self->module();
  return module != NULL ? module : &Node::UNDEFINED_NODE;
}

Node * methodObjectCompose(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  Oper * list = args[0]->asOper();
  M_ASSERT(list != NULL);
  Object * result = new Object(loc, self->asObject(), NULL);
  result->setParentScope(ex->lexicalScope());
  ex->copyParams(result, list);
  return result;
}

void Fundamentals::initObjectType() {
  // Type 'object'
  Object * objectType = TypeRegistry::objectType();
  setAttribute(objectType->name(), objectType);
  if (objectType->attrs().empty()) {
    objectType->defineDynamicAttribute("prototype", objectType, methodObjectPrototype, 0);
    objectType->defineDynamicAttribute("name", TypeRegistry::stringType(), methodObjectName, 0);
    objectType->defineDynamicAttribute("module", TypeRegistry::moduleType(), methodObjectModule, 0);
    objectType->defineDynamicAttribute("parent", TypeRegistry::objectType(), methodObjectParent, 0);
    objectType->defineMethod("compose", "o,params:*o", methodObjectCompose);
  }
}

// -------------------------------------------------------------------------
// Target
// -------------------------------------------------------------------------

Node * methodTargetForSource(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * source = args[0]->requireString();
  Oper * params = args[1]->requireOper();

  M_ASSERT(path::isAbsolute(source->value())) << "Path must be absolute: " << source;
  Object * result = new Object(loc, self->asObject(), NULL);
  result->setParentScope(ex->lexicalScope());
  result->setAttribute(
      strings::str("sources"),
      Oper::createList(source->location(), TypeRegistry::stringListType(), source));
  ex->copyParams(result, params);
  return result;
}

Node * methodTargetForOutput(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * output = args[0]->requireString();
  Oper * params = args[1]->requireOper();

  M_ASSERT(path::isAbsolute(output->value())) << "Path must be absolute: " << output;
  Object * result = new Object(loc, self->asObject(), NULL);
  result->setParentScope(ex->lexicalScope());
  result->setAttribute(
      strings::str("outputs"),
      Oper::createList(output->location(), TypeRegistry::stringListType(), output));
  ex->copyParams(result, params);
  return result;
}

Node * methodOutputDir(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 0);
  return String::create(self->module()->buildDir());
}

void Fundamentals::initTargetType() {

  // Type 'target'
  Object * targetType = TypeRegistry::targetType();
  setAttribute(targetType->name(), targetType);
  if (targetType->attrs().empty()) {
    targetType->setType(TypeRegistry::objectType());
    Type * typeActionList = TypeRegistry::get().getListType(TypeRegistry::actionType());
    Type * typeTargetList = TypeRegistry::get().getListType(TypeRegistry::targetType());

    // Create a type that is a list of files (strings?)
    Node * stringListEmpty = Oper::createEmptyList(TypeRegistry::stringType());
    targetType->defineAttribute("sources", stringListEmpty, TypeRegistry::stringListType(),
        AttributeDefinition::CACHED | AttributeDefinition::PARAM);
    targetType->defineAttribute("outputs", stringListEmpty, TypeRegistry::stringListType(),
        AttributeDefinition::CACHED | AttributeDefinition::PARAM);
    targetType->defineDynamicAttribute("output_dir", TypeRegistry::stringType(), &methodOutputDir,
        AttributeDefinition::CACHED | AttributeDefinition::PARAM);
    targetType->defineAttribute("actions", Oper::createEmptyList(typeActionList), typeActionList);
    targetType->defineAttribute("exclude_from_all", Node::boolFalse(), TypeRegistry::boolType());
    targetType->defineAttribute("source_only", Node::boolFalse(), TypeRegistry::boolType());
    targetType->defineAttribute("internal", Node::boolFalse(), TypeRegistry::boolType());

    targetType->defineAttribute(
        "depends", Oper::createEmptyList(typeTargetList), typeTargetList,
        AttributeDefinition::CACHED | AttributeDefinition::PARAM);
    targetType->defineAttribute(
        "implicit_depends", Oper::createEmptyList(typeTargetList), typeTargetList,
        AttributeDefinition::CACHED);
    targetType->defineMethod("for_source", "t,source:s,params:*o", methodTargetForSource);
    targetType->defineMethod("for_output", "t,output:s,params:*o", methodTargetForOutput);
  }
}

// -------------------------------------------------------------------------
// Option
// -------------------------------------------------------------------------

void Fundamentals::initOptionType() {
  Object * optionType = TypeRegistry::optionType();
  setAttribute(optionType->name(), optionType);
  if (optionType->attrs().empty()) {
    optionType->setName("option");
    optionType->defineDynamicAttribute("name", TypeRegistry::stringType(), methodObjectName);
    optionType->defineAttribute("help", &Node::UNDEFINED_NODE, TypeRegistry::stringType());
    optionType->defineAttribute("abbrev", &Node::UNDEFINED_NODE, TypeRegistry::stringType());
    optionType->defineAttribute("group", &Node::UNDEFINED_NODE, TypeRegistry::stringType());
  }
}

// -------------------------------------------------------------------------
// Action
// -------------------------------------------------------------------------

void Fundamentals::initActionType() {
  // Type 'action'
  Object * actionType = TypeRegistry::actionType();
  setAttribute(actionType->name(), actionType);
  if (actionType->attrs().empty()) {
    //Type * typeObjectList = TypeRegistry::get().getListType(TypeRegistry::actionType());
  }
}

// -------------------------------------------------------------------------
// String
// -------------------------------------------------------------------------

Node * methodStringSize(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 0);
  String * selfStr = self->requireString();
  return Node::makeInt(loc, selfStr->size());
}

Node * methodStringSubstr(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * selfStr = self->requireString();
  int start = args[0]->requireInt();
  int length = args[1]->requireInt();
  return String::create(loc, selfStr->value().substr(start, length));
}

Node * methodStringStartsWith(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * selfStr = self->requireString();
  String * prefixStr = args[0]->requireString();
  return Node::makeBool(selfStr->value().startsWith(prefixStr->value()));
}

Node * methodStringJoin(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * selfStr = self->requireString();
  Oper * argList = args[0]->requireOper();
  size_t size = argList->size() == 0 ? 0 : selfStr->size() * (argList->size() - 1);
  for (Oper::const_iterator it = argList->begin(), itEnd = argList->end(); it != itEnd; ++it) {
    String * arg = (*it)->requireString();
    size += arg->size();
  }
  SmallString<32> result;
  result.reserve(size);
  for (Oper::const_iterator it = argList->begin(), itEnd = argList->end(); it != itEnd; ++it) {
    if (it != argList->begin()) {
      result.append(selfStr->value());
    }
    String * arg = (*it)->requireString();
    result.append(arg->value());
  }
  return String::create(loc, result);
}

void Fundamentals::initStringType() {
  // Type 'string'
  Object * stringMetaType = TypeRegistry::stringMetaType();
  //setAttribute(stringType->name(), stringType);
  if (stringMetaType->attrs().empty()) {
    TypeRegistry::stringType()->setType(stringMetaType);
    stringMetaType->defineDynamicAttribute("size", TypeRegistry::integerType(), methodStringSize, 0);
    stringMetaType->defineMethod(
        "substr", TypeRegistry::stringType(), TypeRegistry::integerType(),
        TypeRegistry::integerType(), methodStringSubstr);
    stringMetaType->defineMethod("starts_with", "b,prefix:s", methodStringStartsWith);
    stringMetaType->defineMethod("join", "s,strings:[s", methodStringJoin);
  }
}

// -------------------------------------------------------------------------
// Host
// -------------------------------------------------------------------------

void Fundamentals::initPlatformVars() {
  Object * platform = createChildScope("platform");
  platform->attrs()[StringRegistry::str(HOST_PLATFORM)] = Node::boolTrue();
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
