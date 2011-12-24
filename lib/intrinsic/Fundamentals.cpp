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
  Node * module = self->module();
  return module != NULL ? module : &Node::UNDEFINED_NODE;
}

Node * methodObjectCompose(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  Oper * list = args[0]->asOper();
  M_ASSERT(list != NULL);
  Object * result = new Object(loc, self->asObject(), NULL);
  Attributes & resultAttrs = result->attrs();
  result->setParentScope(ex->lexicalScope());

  // For every attribute defined in the argument
  Object * objectType = TypeRegistry::objectType();
  for (Node * n = self; n != NULL && n != objectType; n = n->type()) {
    Object * proto = n->asObject();
    if (proto == NULL) {
      continue;
    }
    if (!ex->ensureObjectContents(proto)) {
      continue;
    }
    for (Attributes::iterator it = proto->attrs().begin(), itEnd = proto->attrs().end();
        it != itEnd; ++it) {
      Node * attr = it->second;
      if (attr->nodeKind() == Node::NK_ATTRDEF) {
        // If the current value of the attribute is undefined
        AttributeLookup lookup;
        result->getAttribute(it->first->value(), lookup);
        if (lookup.value->isUndefined() || lookup.value == lookup.definition->value()) {
          // Search arguments until we find one that has that attribute.
          for (Oper::const_iterator ai = list->begin(), aiEnd = list->end(); ai != aiEnd; ++ai) {
            Node * arg = *ai;
            // Copy the value of the argument into the result.
            Node * attrValue = ex->attributeValue(arg, it->first->value());
            if (attrValue != NULL && !attrValue->isUndefined()) {
              resultAttrs[it->first] = attrValue;
              break;
            }
          }
        }
      }
    }
  }
  return result;
}

Node * methodFoldingComposerCompose(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 3);
//  Oper * list = args[0]->asOper();
//  M_ASSERT(list != NULL);
  return NULL;
}

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

Node * functionCaller(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return ex->caller(loc, 4);
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
  defineMethod("command", TypeRegistry::actionType(), TypeRegistry::stringType(),
      TypeRegistry::stringListType(), functionCommand);
  defineDynamicAttribute("caller", TypeRegistry::objectType(), functionCaller);
}

void Fundamentals::initObjectType() {
  // Type 'object'
  Object * objectType = TypeRegistry::objectType();
  setAttribute(objectType->name(), objectType);
  if (objectType->attrs().empty()) {
    Type * typeObjectList = TypeRegistry::get().getListType(TypeRegistry::objectType());

    objectType->defineDynamicAttribute("prototype", objectType, methodObjectPrototype);
    objectType->defineDynamicAttribute("name", TypeRegistry::stringType(), methodObjectName);
    objectType->defineDynamicAttribute("module", TypeRegistry::moduleType(), methodObjectModule);
    objectType->defineDynamicAttribute("parent", TypeRegistry::objectType(), methodObjectParent);
    objectType->defineMethod(
        "compose", TypeRegistry::objectType(), typeObjectList, methodObjectCompose);
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
    Node * stringListEmpty = builder.createListOf(Location(), TypeRegistry::stringType());
    targetType->defineAttribute("sources", stringListEmpty, TypeRegistry::stringListType(),
        AttributeDefinition::CACHED);
    targetType->defineAttribute("outputs", stringListEmpty, TypeRegistry::stringListType(),
        AttributeDefinition::CACHED);

    // Create a type that is a list of targets.
    Node * targetListEmpty = builder.createListOf(Location(), targetType);
    targetType->defineAttribute(
        "depends", targetListEmpty, targetListEmpty->type(), AttributeDefinition::CACHED);
    targetType->defineAttribute(
        "implicit_depends", targetListEmpty, targetListEmpty->type(), AttributeDefinition::CACHED);
  }
}

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

void Fundamentals::initActionType() {
  // Type 'action'
  Object * actionType = TypeRegistry::actionType();
  setAttribute(actionType->name(), actionType);
  if (actionType->attrs().empty()) {
    //Type * typeObjectList = TypeRegistry::get().getListType(TypeRegistry::actionType());
  }
}

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
