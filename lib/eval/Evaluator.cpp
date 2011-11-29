/* ================================================================== *
 * Evaluator
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/graph/Function.h"
#include "mint/graph/Literal.h"
#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/String.h"
#include "mint/graph/TypeRegistry.h"

#include "mint/intrinsic/Fundamentals.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

static inline int cmp(int lhs, int rhs) {
  return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
}

static inline int cmp(double lhs, double rhs) {
  return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
}

Evaluator::Evaluator(Module * module)
  : _module(module)
  , _typeRegistry(module->project()->fundamentals()->typeRegistry())
  , _activeScope(module)
{}

Evaluator::Evaluator(Module * module, TypeRegistry & typeRegistry)
  : _module(module)
  , _typeRegistry(typeRegistry)
  , _activeScope(module)
{}

Node * Evaluator::eval(Node * n) {
//  console::out() << "Evaluating: ";
//  n->dump();
  switch (n->nodeKind()) {
    case Node::NK_UNDEFINED:
    case Node::NK_BOOL:
    case Node::NK_INTEGER:
    case Node::NK_FLOAT:
    case Node::NK_STRING:
    case Node::NK_LIST:
    case Node::NK_DICT:
    case Node::NK_OBJECT:
    case Node::NK_FUNCTION:
    case Node::NK_TYPENAME:
      return n;

    case Node::NK_IDENT: {
      M_ASSERT(_activeScope != NULL);
      String * ident = static_cast<String *>(n);
      for (Node * s = _activeScope; s != NULL; s = s->parentScope()) {
        Node * result = s->getPropertyValue(*ident);
        if (result) {
          Property * prop = s->getPropertyDefinition(*ident);
          if (prop != NULL) {
            if (result == prop) {
              result = prop->value();
            }
            M_ASSERT(result != NULL);
            if (prop->isLazy()) {
              result = eval(result);
            }
          }
          return result;
        }
      }
      diag::error(n->location()) << "Undefined symbol: '" << ident << "'.";
      return &Node::UNDEFINED_NODE;
    }

    case Node::NK_MAKE_LIST:
      return evalList(static_cast<Oper *>(n));

    case Node::NK_MAKE_DICT:
      return evalDict(static_cast<Oper *>(n));

    case Node::NK_MAKE_OBJECT:
      return makeObject(static_cast<Oper *>(n), NULL);

    case Node::NK_SELF:
      return _activeScope;

    case Node::NK_SUPER:
      break;

    // Operations
    case Node::NK_GET_MEMBER: {
      Oper * op = static_cast<Oper *>(n);
      Node * base = eval(op->arg(0));
      if (base->isUndefined()) {
        return &Node::UNDEFINED_NODE;
      }
      String * name = String::dyn_cast(op->arg(1));
      if (name == NULL) {
        diag::error(op->arg(1)->location()) << "Invalid node type for object member: " << op;
        return &Node::UNDEFINED_NODE;
      }
      return evalObjectProperty(name->location(), base, *name);
    }

    case Node::NK_GET_ELEMENT: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      String * key = String::dyn_cast(a1);
      if (key == NULL) {
        diag::error(a1->location()) << "Invalid key type: " << a1->nodeKind();
      }
      if (a0->nodeKind() == Node::NK_DICT) {
        Object * dict = static_cast<Object *>(a0);
        Node * result = dict->getPropertyValue(*key);
        if (result == NULL) {
          diag::error(a1->location()) << "Key error: " << key;
          return &Node::UNDEFINED_NODE;
        }
        return result;
      } else {
        diag::error(op->location()) << "GET_ELEMENT not supported for " << a0->nodeKind();
        return &Node::UNDEFINED_NODE;
      }
      break;
    }

    case Node::NK_CALL:
      return evalCall(static_cast<Oper *>(n));

    // Unary
    case Node::NK_NEGATE: {
      Oper * op = static_cast<Oper *>(n);
      Node * n = eval(op->arg(0));
      M_ASSERT(n != NULL);
      break;
    }

    // Binary
    case Node::NK_ADD: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL && a0->type() != NULL && a0->isConstant());
      M_ASSERT(a1 != NULL && a1->type() != NULL && a1->isConstant());
      if (a0->nodeKind() == Node::NK_FLOAT) {
        M_ASSERT(a1->nodeKind() == Node::NK_FLOAT);
        double v0 = static_cast<const Literal<double> *>(a0)->value();
        double v1 = static_cast<const Literal<double> *>(a1)->value();
        return Node::makeFloat(n->location(), v0 + v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return Node::makeInt(n->location(), v0 + v1);
      } else {
        diag::error(a0->location()) << "Not a number: '" << a0 << "'.";
        return &Node::UNDEFINED_NODE;
      }
    }

    case Node::NK_SUBTRACT: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL && a0->type() != NULL && a0->isConstant());
      M_ASSERT(a1 != NULL && a1->type() != NULL && a1->isConstant());
      if (a0->nodeKind() == Node::NK_FLOAT) {
        M_ASSERT(a1->nodeKind() == Node::NK_FLOAT);
        double v0 = static_cast<const Literal<double> *>(a0)->value();
        double v1 = static_cast<const Literal<double> *>(a1)->value();
        return Node::makeFloat(n->location(), v0 - v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return Node::makeInt(n->location(), v0 - v1);
      } else {
        diag::error(a0->location()) << "Not a number: '" << a0 << "'.";
        return &Node::UNDEFINED_NODE;
      }
    }

    case Node::NK_MULTIPLY: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL && a0->type() != NULL && a0->isConstant());
      M_ASSERT(a1 != NULL && a1->type() != NULL && a1->isConstant());
      if (a0->nodeKind() == Node::NK_FLOAT) {
        M_ASSERT(a1->nodeKind() == Node::NK_FLOAT);
        double v0 = static_cast<const Literal<double> *>(a0)->value();
        double v1 = static_cast<const Literal<double> *>(a1)->value();
        return Node::makeFloat(n->location(), v0 * v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return Node::makeInt(n->location(), v0 * v1);
      } else {
        diag::error(a0->location()) << "Not a number: '" << a0 << "'.";
        return &Node::UNDEFINED_NODE;
      }
    }

    case Node::NK_DIVIDE: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL && a0->type() != NULL && a0->isConstant());
      M_ASSERT(a1 != NULL && a1->type() != NULL && a1->isConstant());
      if (a0->nodeKind() == Node::NK_FLOAT) {
        M_ASSERT(a1->nodeKind() == Node::NK_FLOAT);
        double v0 = static_cast<const Literal<double> *>(a0)->value();
        double v1 = static_cast<const Literal<double> *>(a1)->value();
        return Node::makeFloat(n->location(), v0 / v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return Node::makeInt(n->location(), v0 / v1);
      } else {
        diag::error(a0->location()) << "Not a number: '" << a0 << "'.";
        return &Node::UNDEFINED_NODE;
      }
    }

    case Node::NK_MODULUS: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL && a0->type() != NULL && a0->isConstant());
      M_ASSERT(a1 != NULL && a1->type() != NULL && a1->isConstant());
      if (a0->nodeKind() == Node::NK_FLOAT) {
        diag::error(a1->location()) << "Invalid operand type for modulus: '" << a0 << "'.";
        return &Node::UNDEFINED_NODE;
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return Node::makeInt(n->location(), v0 % v1);
      } else {
        diag::error(a0->location()) << "Not a number: '" << a0 << "'.";
        return &Node::UNDEFINED_NODE;
      }
    }

    case Node::NK_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(equal(op->location(), eval(op->arg(0)), eval(op->arg(1))));
    }

    case Node::NK_NOT_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(!equal(op->location(), eval(op->arg(0)), eval(op->arg(1))));
    }

    case Node::NK_LESS: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0)), eval(op->arg(1))) < 0);
    }

    case Node::NK_LESS_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0)), eval(op->arg(1))) <= 0);
    }

    case Node::NK_GREATER: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0)), eval(op->arg(1))) > 0);
    }

    case Node::NK_GREATER_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0)), eval(op->arg(1))) >= 0);
    }

    case Node::NK_AND: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      if (isNonNil(a0)) {
        Node * a1 = eval(op->arg(1));
        if (isNonNil(a1)) {
          return a1;
        }
        return a0;
      }
      return Node::boolFalse();
    }

    case Node::NK_OR: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      if (isNonNil(a0)) {
        return a0;
      }
      Node * a1 = eval(op->arg(1));
      if (isNonNil(a1)) {
        return a1;
      }
      return Node::boolFalse();
    }

    case Node::NK_MAPS_TO: {
      Oper * op = static_cast<Oper *>(n);
      M_ASSERT(op->size() == 2);
      Node * params = op->arg(0);

      /// The parameter list for this function is either a single parameter name or a tuple
      /// of parameter names.
      NodeArray paramNodes;
      if (params->nodeKind() == Node::NK_MAKE_TUPLE) {
        paramNodes = static_cast<Oper *>(params)->args();
      } else {
        paramNodes = makeArrayRef(params);
      }

      SmallVector<Type *, 4> paramTypes;
      SmallVector<Parameter, 4> paramNames;

      for (NodeArray::const_iterator it = paramNodes.begin(), itEnd = paramNodes.end();
          it != itEnd; ++it) {
        Node * p = *it;
        if (p->nodeKind() == Node::NK_IDENT) {
          String * pname = static_cast<String *>(p);
          paramNames.push_back(Parameter(pname));
          paramTypes.push_back(TypeRegistry::anyType());
        } else {
          diag::error(p->location()) << "Invalid function parameter definition";
        }
      }

      DerivedType * fnType = _typeRegistry.getFunctionType(TypeRegistry::anyType(), paramTypes);
      Function * func = new Function(
          Node::NK_FUNCTION, op->location(), fnType, paramNames, evalFunctionBody);
      func->setBody(op->arg(1));
      func->setParentScope(_activeScope);
      return func;
    }

    case Node::NK_CONCAT:
      return evalConcat(static_cast<Oper *>(n));

    case Node::NK_DO:
      return evalDoStmt(static_cast<Oper *>(n));

    case Node::NK_LET:
      return evalLetStmt(static_cast<Oper *>(n));

    case Node::NK_MAKE_MODULE:
    case Node::NK_MODULE:
    case Node::NK_PROJECT: {
      console::err() << "Expression cannot be evaluated: " << n->nodeKind();
      return NULL;
    }

    default: {
      M_ASSERT(false) << "Undefined node type: " << unsigned(n->nodeKind());
      return NULL;
    }
  }

  M_ASSERT(false) << "Invalid state for nodeKind " << n->nodeKind();
  return NULL;
}

bool Evaluator::evalModuleContents(Oper * content) {
  NodeArray::const_iterator it = content->begin(), itEnd = content->end();
  Node * savedScope = setActiveScope(_module);
  while (it != itEnd) {
    Node * n = *it++;
    switch (n->nodeKind()) {
      case Node::NK_SET_MEMBER: {
        if (!evalModuleProperty(static_cast<Oper *>(n))) {
          setActiveScope(savedScope);
          return false;
        }
        break;
      }

      case Node::NK_MAKE_OPTION: {
        if (!evalModuleOption(static_cast<Oper *>(n))) {
          setActiveScope(savedScope);
          return false;
        }
        break;
      }

      case Node::NK_MAKE_ACTION: {
        Oper * action = static_cast<Oper *>(n);
        M_ASSERT(action->size() == 1);
        Node * value = eval(action->arg(0));
        if (value != NULL) {
          _module->addAction(value);
        }
        break;
      }

      case Node::NK_IMPORT: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() == 1);
        Module * m = importModule(importOp->arg(0));
        if (m != NULL) {
          //_module->addImportScope(m);
          // This one is tricky - we want to preserve the entire relative path
          // between the two modules.
          M_ASSERT(false) << "implement";
        }
        break;
      }

      case Node::NK_IMPORT_AS: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() == 2);
        Module * m = importModule(importOp->arg(0));
        if (m != NULL) {
          /// Create a scope to store the module by the 'as' name.
          Object * newScope = new Object(Node::NK_DICT, importOp->location(), NULL);
          String * asName = String::cast(importOp->arg(1));
          newScope->properties()[asName] = m;
          _module->addImportScope(newScope);
        }
        M_ASSERT(false) << "implement";
        break;
      }

      case Node::NK_IMPORT_FROM: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() > 1);
        Module * m = importModule(importOp->arg(0));
        if (m != NULL) {
          // We're not importing the entire module, so create a special import scope
          // to hold just the symbols we want.
          Object * newScope = new Object(Node::NK_DICT, importOp->location(), NULL);
          for (Oper::const_iterator it = importOp->begin() + 1, itEnd = importOp->end();
              it != itEnd; ++it) {
            String * symName = String::cast(*it);
            Node * value = m->getPropertyValue(*symName);
            if (value == NULL) {
              diag::error(symName->location()) << "Undefined symbol '" << symName << "'.";
              return NULL;
            }
            newScope->properties()[symName] = value;
          }
          _module->addImportScope(newScope);
        }
        break;
      }

      case Node::NK_IMPORT_ALL: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() == 1);
        Module * m = importModule(importOp->arg(0));
        if (m != NULL) {
          _module->addImportScope(m);
        }
        break;
      }

      default:
        diag::error(n->location()) << "Invalid expression for module property: '" << n << "'.";
        setActiveScope(savedScope);
        return false;
    }
  }

  setActiveScope(savedScope);
  return true;
}

bool Evaluator::evalModuleProperty(Oper * op) {
  Node * propName = op->arg(0);
  if (propName->nodeKind() == Node::NK_IDENT) {
    String * ident = static_cast<String *>(propName);
    if (!checkModulePropertyDefined(ident)) {
      return false;
    }
    Node * propValue = op->arg(1);
    if (propValue->nodeKind() == Node::NK_MAKE_OBJECT) {
      propValue = makeObject(static_cast<Oper *>(propValue), ident);
    } else {
      propValue = eval(propValue);
    }
    _module->setProperty(ident, propValue);
  } else {
    diag::error(op->location()) << "Invalid expression for module property name: '"
        << propName << "'.";
    return false;
  }
  return true;
}

bool Evaluator::evalModuleOption(Oper * op) {
  M_ASSERT(op != NULL);
  M_ASSERT(op->size() >= 2);
  Node * optName = op->arg(0);
  Node * optType = op->arg(1);
  if (optName->nodeKind() != Node::NK_IDENT) {
    diag::error(optName->location()) << "Invalid option name: '" << optName << "'.";
  }
  String * optNameStr = static_cast<String *>(optName);

  // An 'option' object derives from the special 'option' prototype.
  Fundamentals * fundamentals = this->fundamentals();
  Object * obj = new Object(
      Node::NK_OPTION,
      op->location(),
      static_cast<Object *>(fundamentals->option));
  obj->setName(optNameStr);
  obj->setType(evalTypeExpression(optType));

  if (!checkModulePropertyDefined(optNameStr)) {
    return false;
  }
  obj->properties()[fundamentals->str("name")] = optNameStr;

  // Unlike regular objects, options don't define their own namespace, so we don't change
  // activeScope here.

  // Evaluate all of the properties.
  NodeArray::const_iterator it = op->begin() + 2, itEnd = op->end();
  for (; it != itEnd; ++it) {
    Node * n = *it;
    M_ASSERT(n->nodeKind() == Node::NK_SET_MEMBER);
    Oper * setOp = static_cast<Oper *>(n);
    M_ASSERT(setOp->size() == 2);
    String * propNameStr = String::dyn_cast(setOp->arg(0));
    if (propNameStr == NULL) {
      diag::error(optName->location()) << "Invalid option name: '" << optName << "'.";
    }
    Node * propValue = setOp->arg(1);
    if (propNameStr->value() == "default") {
      // Default is handled specially because it varies in type.
      propValue = eval(propValue);
      obj->properties()[propNameStr] = propValue;
    } else {
      setObjectProperty(obj, propNameStr, propValue);
    }
  }

  _module->setProperty(optNameStr, obj);
  return true;
}

bool Evaluator::checkModulePropertyDefined(String * propName) {
  PropertyTable::const_iterator prev = _module->properties().find(propName);
  if (prev != _module->properties().end()) {
    diag::error(propName->location()) << "Property '" << propName
        << "' is already defined in this module";
    diag::info(prev->second->location()) << "at this location.";
    return false;
  }
  return true;
}

bool Evaluator::evalObjectContents(Object * obj) {
  Node * savedScope = setActiveScope(obj);
  Oper * definition = static_cast<Oper *>(obj->definition());
  obj->clearDefinition();
  if (obj->prototype() != NULL && obj->prototype()->definition() != NULL) {
    if (!evalObjectContents(obj->prototype())) {
      return false;
    }
  }
  bool success = true;
  M_ASSERT(definition->size() >= 1);
  for (Oper::const_iterator
      it = definition->begin() + 1, itEnd = definition->end(); it != itEnd; ++it) {
    Node * n = *it;
    switch (n->nodeKind()) {
      case Node::NK_SET_MEMBER: {
        Oper * op = static_cast<Oper *>(n);
        Node * propName = op->arg(0);
        Node * propValue = op->arg(1);
        if (propName->nodeKind() != Node::NK_IDENT) {
          diag::error(propName->location()) << "Invalid expression for object property name '"
              << propName << "'.";
          success = false;
          continue;
        }
        setObjectProperty(obj, static_cast<String *>(propName), propValue);
        break;
      }

      case Node::NK_MAKE_PARAM: {
        Oper * op = static_cast<Oper *>(n);
        Node * propName = op->arg(0);
        if (propName->nodeKind() != Node::NK_IDENT) {
          diag::error(propName->location()) << "Invalid expression for object property name '"
              << propName << "'.";
          success = false;
          continue;
        }
        Property * propDef = obj->getPropertyDefinition(*static_cast<String *>(propName));
        if (propDef != NULL) {
          diag::error(propName->location()) << "Property '" << propName
              << "' is already defined on object '" << obj->nameSafe() << "'.";
          diag::info(propDef->location()) << "Previous property definition.";
        }
        String * name = static_cast<String *>(propName);
        Type * type = evalTypeExpression(op->arg(1));
        Node * value = op->arg(2);
        Literal<int> * flags = static_cast<Literal<int> *>(op->arg(3));
        if (!(flags->value() & Property::LAZY)) {
          value = eval(value);
          M_ASSERT(value != NULL) << "Evaluation of " << op->arg(2) << " returned NULL";
        }
        if (type == NULL) {
          type = value->type();
          M_ASSERT(type != NULL);
        }
        Property * prop = new Property(value, type, flags->value());
        obj->properties()[name] = prop;
        break;
      }

      default:
        diag::error(n->location()) << "Invalid expression for object property: '" << n << "'.";
        setActiveScope(savedScope);
        return false;
    }
  }

  setActiveScope(savedScope);
  return success;
}

//Node * Evaluator::realizeObjectProperty(Location loc, Object * obj, StringRef name) {
//  if (!obj->hasPropertyImmediate(name)) {
//    Node * value = evalObjectProperty(loc, obj, name);
//    obj->properties()[String::createIdent(name)] = value;
//    if (value->nodeKind() == Node::NK_OBJECT) {
//      evalObjectContents(static_cast<Object *>(value));
//    }
//  }
//  return obj->getPropertyValue(name);
//}

Node * Evaluator::evalObjectProperty(Location loc, Node * base, StringRef name) {
  Node * value = base->getPropertyValue(name);
  if (value == NULL) {
    diag::error(loc) << "Undefined symbol for object '" << base << "': " << name;
    return &Node::UNDEFINED_NODE;
  }
  // If it's a lazy property, then evaluate it now.
  if (base->nodeKind() == Node::NK_OBJECT) {
    Object * obj = static_cast<Object *>(base);
    if (obj->definition() != NULL) {
      evalObjectContents(obj);
    }
    Property * propDef = obj->getPropertyDefinition(name);
    if (propDef != NULL) {
      if (value == propDef) {
        value = propDef->value();
      }
      if (value != NULL && propDef->isLazy()) {
        Node * savedScope = setActiveScope(obj);
        value = eval(value);
        setActiveScope(savedScope);
      }
    }
  }
  return value;
}

Type * Evaluator::evalTypeExpression(Node * ty) {
  if (ty == NULL) {
    return NULL;
  } else if (ty->nodeKind() == Node::NK_TYPENAME || ty->nodeKind() == Node::NK_OBJECT) {
    return static_cast<Type *>(ty);
  } else if (ty->nodeKind() == Node::NK_IDENT) {
    Node * n = eval(ty);
    M_ASSERT(n != NULL);
    if (n->nodeKind() == Node::NK_OBJECT) {
      return static_cast<Type *>(n);
    }
  } else if (ty->nodeKind() == Node::NK_GET_ELEMENT) {
    Oper * op = static_cast<Oper *>(ty);
    Type * base = evalTypeExpression(op->arg(0));
    if (base->isUndefined()) {
      return TypeRegistry::undefinedType();
    }
    if (base == TypeRegistry::genericListType()) {
      if (op->size() != 2) {
        diag::error(ty->location()) << "Incorrect number of type parameters for list type";
        return TypeRegistry::undefinedType();
      }
      Type * param = evalTypeExpression(op->arg(1));
      if (param->isUndefined()) {
        return TypeRegistry::undefinedType();
      }
      return _typeRegistry.getListType(param);
    } else if (base == TypeRegistry::genericDictType()) {
      if (op->size() != 3) {
        diag::error(ty->location()) << "Incorrect number of type parameters for list type";
        return TypeRegistry::undefinedType();
      }
      Type * keyType = evalTypeExpression(op->arg(1));
      Type * valueType = evalTypeExpression(op->arg(2));
      if (keyType->isUndefined()) {
        return TypeRegistry::undefinedType();
      }
      if (valueType->isUndefined()) {
        return TypeRegistry::undefinedType();
      }
      return _typeRegistry.getDictType(keyType, valueType);
    }
  }
  diag::error(ty->location()) << "'" << ty << "' is not a type name.";
  return TypeRegistry::undefinedType();
}

Node * Evaluator::evalList(Oper * op) {
  SmallVector<Node *, 32> args;
  args.resize(op->size());

  NodeArray::const_iterator src = op->begin(), srcEnd = op->end();
  SmallVector<Node *, 32>::iterator dst = args.begin();
  Type * elementType = NULL;
  while (src < srcEnd) {
    Node * n = eval(*src++);
    elementType = selectCommonType(elementType, n->type());
    *dst++ = n;
  }

  Oper * result = Oper::create(Node::NK_LIST, op->location(), op->type(), args);
  if (elementType != NULL) {
    result->setType(_typeRegistry.getListType(elementType));
  } else {
    // TODO: Factor in 'expected' type.
    result->setType(_typeRegistry.genericListType());
  }
  return result;
}

Node * Evaluator::evalDict(Oper * op) {
  SmallVector<Node *, 32> args;
  args.resize(op->size());
  evalArgs(op->args().begin(), args.begin(), op->size());
  M_ASSERT((op->size() & 1) == 0);
  Object * result = new Object(Node::NK_DICT, op->location(), NULL);
  for (Oper::const_iterator it = args.begin(), itEnd = args.end(); it != itEnd;) {
    Node * key = *it++;
    Node * value = *it++;
    String * strKey = String::dyn_cast(key);
    if (strKey == NULL) {
      diag::error(key->location()) << "Only string keys are supported for dictionary types";
      continue;
    }
    result->properties()[strKey] = value;
  }
  return result;
}

Node * Evaluator::evalCall(Oper * op) {
  // First arg is handled differently
  Node * callable = op->arg(0);
  Node * func = NULL;
  Node * self = NULL;
  if (callable->nodeKind() == Node::NK_IDENT) {
    M_ASSERT(_activeScope != NULL);
    String * name = static_cast<String *>(callable);
    for (Node * s = _activeScope; s != NULL; s = s->parentScope()) {
      func = s->getPropertyValue(*name);
      if (func) {
        self = s;
        break;
      }
    }

    if (!func) {
      diag::error(callable->location()) << "Undefined symbol: '" << name << "'.";
      return &Node::UNDEFINED_NODE;
    }
  } else if (callable->nodeKind() == Node::NK_GET_MEMBER) {
    Oper * getMemberOp = static_cast<Oper *>(callable);
    self = eval(getMemberOp->arg(0));
    if (self->nodeKind() == Node::NK_UNDEFINED) {
      return &Node::UNDEFINED_NODE;
    }
    String * name = static_cast<String *>(getMemberOp->arg(1));
    if (self->nodeKind() == Node::NK_LIST) {
      func = fundamentals()->list->getPropertyValue(*name);
    } else {
      func = self->getPropertyValue(*name);
    }
    if (!func) {
      diag::error(callable->location()) << "Undefined symbol: '" << name << "'.";
      return &Node::UNDEFINED_NODE;
    }
  } else {
    callable = eval(callable);
    if (callable->nodeKind() == Node::NK_UNDEFINED) {
      return &Node::UNDEFINED_NODE;
    }
  }

  SmallVector<Node *, 32> args;
  args.resize(op->size() - 1);
  evalArgs(op->args().begin() + 1, args.begin(), op->size() - 1);

  if (func->nodeKind() == Node::NK_FUNCTION) {
    Function * fn = static_cast<Function *>(func);
    if (!coerceArgs(fn, args)) {
      return &Node::UNDEFINED_NODE;
    }
    return (*fn->handler())(this, fn, self, NodeArray(args));
  } else {
    diag::error(callable->location()) << "Expression is not a function: '" << callable << "'.";
    return &Node::UNDEFINED_NODE;
  }
}

Node * Evaluator::evalConcat(Oper * op) {
  // First arg is handled differently
  SmallVector<Node *, 32> args;
  args.resize(op->size());
  evalArgs(op->args().begin(), args.begin(), op->size());

  unsigned numStringArgs = 0;
  Type * listType = NULL;
  for (Oper::iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    Node * n = *it;
    if (n->type() != NULL) {
      if (n->type()->typeKind() == Type::STRING) {
        ++numStringArgs;
      } else if (n->type()->typeKind() == Type::LIST) {
        listType = selectCommonType(listType, n->type());
      }
    }
  }

  if (listType != 0) {
    // List concatenation
    SmallVector<Node *, 32> result;
    for (Oper::iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
      Node * n = *it;
      Node * coercedValue = coerce(n, listType);
      if (coercedValue == NULL || coercedValue->nodeKind() != Node::NK_LIST) {
        diag::error(n->location()) << "Attempt to concatenate non-list type: " << n;
      } else {
        Oper * listValue = static_cast<Oper *>(coercedValue);
        result.append(listValue->begin(), listValue->end());
      }
    }
    return Oper::create(Node::NK_LIST, op->location(), listType, result);
  } else if (numStringArgs > 0) {
    // String concatenation
    SmallString<128> result;
    for (Oper::iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
      Node * n = *it;
      Node * coercedValue = coerce(n, TypeRegistry::stringType());
      if (coercedValue == NULL) {
        diag::error(n->location()) << "Value cannot be converted to string: " << n;
      } else {
        String * s = String::cast(coercedValue);
        result.append(s->value().begin(), s->value().end());
      }
    }
    return String::create(op->location(), result);
  } else {
    diag::error(op->location()) << "Invalid type for concatenate operation: " << op;
    return &Node::UNDEFINED_NODE;
  }
}

Node * Evaluator::evalDoStmt(Oper * op) {
  Node * result = &Node::UNDEFINED_NODE;
  for (Oper::const_iterator it = op->begin(), itEnd = op->end(); it != itEnd; ++it) {
    result = eval(*it);
  }
  // Return the value of the last evaluated node.
  return result;
}

Node * Evaluator::evalLetStmt(Oper * op) {
  Node * result = &Node::UNDEFINED_NODE;
  Object * localScope = new Object(Node::NK_DICT, op->location(), NULL);
  localScope->setParentScope(_activeScope);
  Node * savedScope = setActiveScope(localScope);
  M_ASSERT(op->size() > 1);
  // Set up the local environment.
  for (Oper::const_iterator it = op->begin(), itEnd = op->end() - 1; it != itEnd; ++it) {
    Oper * setOp = static_cast<Oper *>(*it);
    M_ASSERT(setOp->nodeKind() == Node::NK_SET_MEMBER);
    String * propName = String::cast(setOp->arg(0));
    Node * propValue = eval(setOp->arg(1));
    localScope->properties()[propName] = propValue;
  }
  result = eval(*(op->end() - 1));
  setActiveScope(savedScope);
  return result;
}

Node * Evaluator::makeObject(Oper * op, String * name) {
  M_ASSERT(op->size() >= 1);
  Node * protoExpr = op->arg(0);
  Node * prototype = eval(protoExpr);
  if (prototype->nodeKind() == Node::NK_UNDEFINED) {
    return &Node::UNDEFINED_NODE;
  } else if (prototype->nodeKind() != Node::NK_OBJECT) {
    diag::error(protoExpr->location()) << "Prototype expression is not an object: '"
        << protoExpr << "'.";
    return &Node::UNDEFINED_NODE;
  }

  Object * obj = new Object(op->location(), static_cast<Object *>(prototype), op);
  obj->setParentScope(_activeScope);
  obj->setType(obj);
  if (name != NULL) {
    obj->setName(name);
  }
  return obj;
}

bool Evaluator::setObjectProperty(Object * obj, String * propName, Node * propValue) {
  String * propNameStr = static_cast<String *>(propName);
  Property * propDef = obj->getPropertyDefinition(*propNameStr);
  if (propDef == NULL) {
    diag::error(propName->location()) << "Attempt to set non-existent property '"
        << propName << "' on object '" << obj->nameSafe() << "'.";
    return false;
  }

  PropertyTable::const_iterator pi = obj->properties().find(propNameStr);
  if (pi != obj->properties().end()) {
    diag::error(propName->location()) << "Property '" << propName
        << "' has already be defined on object '" << obj->nameSafe() << "'.";
    return false;
  }

  if (propValue->nodeKind() == Node::NK_MAKE_OBJECT) {
    propValue = makeObject(static_cast<Oper *>(propValue), propNameStr);
  } else if (!propDef->isLazy()) {
    propValue = eval(propValue);
  }
  if (propValue == NULL) {
    return false;
  }
  if (!propDef->isLazy() && propDef->type() != NULL) {
    Node * coercedValue = coerce(propValue, propDef->type());
    if (coercedValue == NULL) {
      return false;
    }
    propValue = coercedValue;
  }

  obj->properties()[propNameStr] = propValue;
  return true;
}

void Evaluator::evalArgs(NodeArray::iterator src, Node ** dst, size_t count) {
  while (count--) {
    *dst++ = eval(*src++);
  }
}

bool Evaluator::equal(Location loc, Node * lhs, Node * rhs) {
  M_ASSERT(lhs != NULL);
  M_ASSERT(rhs != NULL);
  if (lhs == rhs) {
    return true;
  }
  switch (lhs->nodeKind()) {
    case Node::NK_UNDEFINED:
      return rhs->nodeKind() == Node::NK_UNDEFINED;

    case Node::NK_BOOL: {
      if (rhs->nodeKind() != Node::NK_BOOL) {
        break;
      }
      Literal<bool> * ll = static_cast<Literal<bool> *>(lhs);
      Literal<bool> * rl = static_cast<Literal<bool> *>(rhs);
      return ll->value() == rl->value();
    }

    case Node::NK_INTEGER: {
      Literal<int> * ll = static_cast<Literal<int> *>(lhs);
      if (rhs->nodeKind() == Node::NK_INTEGER) {
        Literal<int> * rl = static_cast<Literal<int> *>(rhs);
        return ll->value() == rl->value();
      } else if (rhs->nodeKind() == Node::NK_FLOAT) {
        Literal<double> * rl = static_cast<Literal<double> *>(rhs);
        return double(ll->value()) == rl->value();
      }
      break;
    }

    case Node::NK_FLOAT: {
      Literal<double> * ll = static_cast<Literal<double> *>(lhs);
      if (rhs->nodeKind() == Node::NK_INTEGER) {
        Literal<int> * rl = static_cast<Literal<int> *>(rhs);
        return ll->value() == double(rl->value());
      } else if (rhs->nodeKind() == Node::NK_FLOAT) {
        Literal<double> * rl = static_cast<Literal<double> *>(rhs);
        return ll->value() == rl->value();
      }
      break;
    }

    case Node::NK_STRING: {
      if (rhs->nodeKind() != Node::NK_STRING) {
        break;
      }
      String * lstr = static_cast<String *>(lhs);
      String * rstr = static_cast<String *>(rhs);
      return lstr->value().equals(rstr->value());
    }

    case Node::NK_LIST: {
      if (rhs->nodeKind() != Node::NK_LIST) {
        return false;
      }
      Oper * llist = static_cast<Oper *>(lhs);
      Oper * rlist = static_cast<Oper *>(rhs);
      if (llist->size() != rlist->size()) {
        return false;
      }
      for (unsigned i = 0, len = llist->size(); i < len; ++i) {
        if (!equal(loc, llist->arg(i), rlist->arg(i))) {
          return false;
        }
      }
      return true;
    }

    default:
      break;
  }

  diag::error(loc) << "Incomparable types: " << lhs->type() << " and " << rhs->type();
  return false;
}

int Evaluator::compare(Location loc, Node * lhs, Node * rhs) {
  M_ASSERT(lhs != NULL);
  M_ASSERT(rhs != NULL);
  if (lhs == rhs) {
    return 0;
  }
  switch (lhs->nodeKind()) {
    case Node::NK_UNDEFINED:
      return rhs->nodeKind() == Node::NK_UNDEFINED ? 0 : -1;

    case Node::NK_BOOL: {
      if (rhs->nodeKind() != Node::NK_BOOL) {
        break;
      }
      Literal<bool> * ll = static_cast<Literal<bool> *>(lhs);
      Literal<bool> * rl = static_cast<Literal<bool> *>(rhs);
      if (ll->value()) {
        return rl->value() ? 0 : 1;
      } else {
        return rl->value() ? -1 : 0;
      }
    }

    case Node::NK_INTEGER: {
      Literal<int> * ll = static_cast<Literal<int> *>(lhs);
      if (rhs->nodeKind() == Node::NK_INTEGER) {
        Literal<int> * rl = static_cast<Literal<int> *>(rhs);
        return cmp(ll->value(), rl->value());
      } else if (rhs->nodeKind() == Node::NK_FLOAT) {
        Literal<double> * rl = static_cast<Literal<double> *>(rhs);
        return cmp(double(ll->value()), rl->value());
      }
      break;
    }

    case Node::NK_FLOAT: {
      Literal<double> * ll = static_cast<Literal<double> *>(lhs);
      if (rhs->nodeKind() == Node::NK_INTEGER) {
        Literal<int> * rl = static_cast<Literal<int> *>(rhs);
        return cmp(ll->value(), double(rl->value()));
      } else if (rhs->nodeKind() == Node::NK_FLOAT) {
        Literal<double> * rl = static_cast<Literal<double> *>(rhs);
        return cmp(ll->value(), rl->value());
      }
      break;
    }

    case Node::NK_STRING: {
      if (rhs->nodeKind() != Node::NK_STRING) {
        break;
      }
      String * lstr = static_cast<String *>(lhs);
      String * rstr = static_cast<String *>(rhs);
      return lstr->value().compare(rstr->value());
    }

#if 0
    case Node::NK_LIST: {
      if (rhs->nodeKind() != Node::NK_LIST) {
        return false;
      }
      Oper * llist = static_cast<Oper *>(lhs);
      Oper * rlist = static_cast<Oper *>(rhs);
      if (llist->size() != rlist->size()) {
        return false;
      }
      for (unsigned i = 0, len = llist->size(); i < len; ++i) {
        if (!equal(loc, llist->arg(i), rlist->arg(i))) {
          return false;
        }
      }
      return true;
    }
#endif

    default:
      break;
  }

  diag::error(loc) << "Incomparable types: " << lhs->type() << " and " << rhs->type();
  return -1;
}

bool Evaluator::isNonNil(Node * n) {
  switch (n->nodeKind()) {
    case Node::NK_UNDEFINED:
      return false;

    case Node::NK_BOOL: {
      Literal<bool> * lit = static_cast<Literal<bool> *>(n);
      return lit->value();
    }

    case Node::NK_INTEGER: {
      Literal<int> * lit = static_cast<Literal<int> *>(n);
      return lit->value() != 0;
    }

    case Node::NK_FLOAT: {
      Literal<double> * lit = static_cast<Literal<double> *>(n);
      return lit->value() != 0.0;
    }

    case Node::NK_STRING: {
      String * str = static_cast<String *>(n);
      return !str->value().empty();
    }

    case Node::NK_LIST: {
      Oper * op = static_cast<Oper *>(n);
      return op->size() != 0;
    }

    case Node::NK_DICT: {
      Object * dict = static_cast<Object *>(n);
      return dict->properties().size() != 0;
    }

    default:
      return true;
  }
}

bool Evaluator::coerceArgs(Function * fn, SmallVectorImpl<Node *> & args) {
  // TODO: Modify this to handle varargs functions.
  M_ASSERT(fn->type()->typeKind() == Type::FUNCTION);
  DerivedType * fnType = static_cast<DerivedType *>(fn->type());
  M_ASSERT(fnType->size() >= 1);
  unsigned argIndex = 0;
  bool success = true;
  for (SmallVectorImpl<Node *>::iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it) {
    Type * argType = fnType->params()[argIndex + 1];
    Node * arg = coerce(*it, argType);
    if (arg == NULL) {
      success = false;
    } else {
      *it = arg;
    }
    ++argIndex;
  }
  return success;
}

Node * Evaluator::coerce(Node * n, Type * ty) {
  M_ASSERT(n != NULL);
  M_ASSERT(ty != NULL);
  M_ASSERT(n->type() != NULL) << "Node '" << n << "' has no type information!";
  if (n->type() == ty) {
    return n;
  } else if (n->nodeKind() == Node::NK_UNDEFINED && ty->typeKind() != Type::ANY) {
    return NULL;
  }
  M_ASSERT(n->isConstant()) << "Expression '" << n << ". is not a constant.";
  Type * srcType = n->type();
  //Type::TypeKind srcKind = srcType->typeKind();
  switch (ty->typeKind()) {
    case Type::ANY:
      return n;
    case Type::VOID:
      break;
    case Type::BOOL:
      break;
    case Type::INTEGER: {
      if (n->nodeKind() == Node::NK_FLOAT) {
        double v0 = static_cast<const Literal<double> *>(n)->value();
        return Node::makeInt(n->location(), int(v0));
      }
      break;
    }
    case Type::FLOAT: {
      if (n->nodeKind() == Node::NK_INTEGER) {
        int v0 = static_cast<const Literal<int> *>(n)->value();
        return Node::makeFloat(n->location(), v0);
      }
      break;
    }
    case Type::STRING: {
      switch (n->nodeKind()) {
        case Node::NK_UNDEFINED:
          return String::strUndefined();

        case Node::NK_BOOL: {
          Literal<bool> * boolValue = static_cast<Literal<bool> *>(n);
          return boolValue->value() ? String::strTrue() : String::strFalse();
        }

        case Node::NK_INTEGER: {
          Literal<int> * intValue = static_cast<Literal<int> *>(n);
          OStrStream strm;
          strm << intValue->value();
          return String::create(strm.str());
        }

        case Node::NK_FLOAT: {
          Literal<double> * floatValue = static_cast<Literal<double> *>(n);
          OStrStream strm;
          strm << floatValue->value();
          return String::create(strm.str());
        }

        default:
          break;
      }
      break;
    }

    case Type::LIST: {
      DerivedType * listType = static_cast<DerivedType *>(ty);
      Type * elementType = listType->params()[0];
      if (n->nodeKind() == Node::NK_LIST) {
        Oper * list = static_cast<Oper *>(n);
        SmallVector<Node *, 32> elements;
        elements.resize(list->size());
        SmallVector<Node *, 32>::iterator put = elements.begin();
        bool changed = false;
        for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
          Node * el = coerce(*it, elementType);
          if (el != *it) {
            changed = true;
          }
          *put++ = el;
        }

        if (!changed) {
          return n;
        } else {
          return Oper::create(Node::NK_LIST, n->location(), ty, elements);
        }
      }
      break;
    }

    case Type::DICTIONARY:
      break;

    case Type::OBJECT: {
      if (n->nodeKind() == Node::NK_OBJECT) {
        Object * obj = static_cast<Object *>(n);
        if (obj->inheritsFrom(static_cast<Object *>(ty))) {
          return n;
        }
      }
      break;
    }

    case Type::FUNCTION:
      break;

    default:
      diag::error(n->location()) << "Invalid type for coercion: " << ty->typeKind();
      return NULL;
  }

  diag::error(n->location()) << "Cannot coerce value of type " << srcType << " to " << ty;
  return NULL;
}

Type * Evaluator::selectCommonType(Type * t0, Type * t1) {
  if (t0 == NULL || t0 == t1) {
    return t1;
  } else if (t1 == NULL) {
    return t0;
  } else {
    switch (t0->typeKind()) {
      case Type::ANY:
        return t0;

      case Type::VOID:
        return t1;

      case Type::BOOL:
      case Type::INTEGER:
      case Type::FLOAT:
      case Type::STRING:
      case Type::FUNCTION:
        return TypeRegistry::anyType();

      case Type::LIST: {
        if (t1->typeKind() != Type::LIST) {
          return TypeRegistry::anyType();
        }
        DerivedType * dt0 = static_cast<DerivedType *>(t0);
        DerivedType * dt1 = static_cast<DerivedType *>(t1);
        return _typeRegistry.getListType(selectCommonType(dt0->params()[0], dt1->params()[0]));
      }

      case Type::DICTIONARY: {
        M_ASSERT(false) << "Implement";
        return TypeRegistry::anyType();
      }

      case Type::OBJECT: {
        if (t1->typeKind() != Type::OBJECT) {
          return TypeRegistry::anyType();
        }
        Object * obj0 = static_cast<Object *>(t0);
        Object * obj1 = static_cast<Object *>(t1);
        if (obj0->inheritsFrom(obj1)) {
          return obj1;
        }
        if (obj1->inheritsFrom(obj0)) {
          return obj0;
        }
        return TypeRegistry::anyType();
      }

      default:
        M_ASSERT(false) << "Invalid type: " << t0->typeKind();
        return NULL;
    }
  }
}

Node * Evaluator::evalFunctionBody(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  Object * localScope = new Object(fn->location(), NULL, NULL);
  localScope->setParentScope(fn->parentScope());
  M_ASSERT(args.size() == fn->argCount());
  SmallVectorImpl<Parameter>::const_iterator pi = fn->params().begin();
  for (NodeArray::const_iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it, ++pi) {
    const Parameter & param = *pi;
    localScope->properties()[param.name()] = *it;
  }
  Node * savedScope = ex->setActiveScope(localScope);
  Node * result = ex->eval(fn->body());
  ex->setActiveScope(savedScope);
  return result;
}

Module * Evaluator::importModule(Node * path) {
  M_ASSERT(path != NULL);
  M_ASSERT(_module != NULL);
  M_ASSERT(_module->project() != NULL);
  StringRef pathStr = String::cast(path)->value();

  // The project from which we are going to load from.
  Project * project = _module->project();

  size_t dotPos = pathStr.find('.');
  size_t colonPos = pathStr.find(':');
  if (colonPos < dotPos) {
    StringRef projName = pathStr.substr(0, colonPos);
    project = _module->project()->buildConfig()->getProject(projName);
    if (project == NULL) {
      diag::error(path->location()) << "Project '" << projName << "' not found.";
      return NULL;
    }
    pathStr = pathStr.substr(colonPos + 1);
    return project->loadModule(pathStr);
  } else if (_module->moduleName().empty()) {
    return project->loadModule(pathStr);
  } else {
    SmallString<64> combinedPath(pathStr);
    dotPos = StringRef(combinedPath).rfind('.');
    if (dotPos != StringRef::npos) {
      combinedPath.erase(combinedPath.begin() + dotPos + 1, combinedPath.end());
    }
    combinedPath.append(pathStr.begin(), pathStr.end());
    return project->loadModule(combinedPath);
  }
}

Fundamentals * Evaluator::fundamentals() const {
  return _module->project()->fundamentals();
}

}
