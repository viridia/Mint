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

#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

Evaluator::Evaluator(Module * module)
  : _module(module)
  , _activeScope(module)
{}

Node * Evaluator::eval(Node * n) {
  console::out() << "Evaluating: ";
  n->dump();
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
      String * value = static_cast<String *>(n);
      for (Node * s = _activeScope; s != NULL; s = s->parentScope()) {
        Node * result = s->getPropertyValue(value);
        if (result) {
          return result;
        }
      }
      diag::error(n->location()) << "Undefined symbol: '" << value << "'.";
      return &Node::UNDEFINED_NODE;
    }

    case Node::NK_MAKE_LIST:
      return evalList(static_cast<Oper *>(n));

    case Node::NK_MAKE_DICT:
      return evalDict(static_cast<Oper *>(n));

    case Node::NK_MAKE_OBJECT:
      return makeObject(static_cast<Oper *>(n), NULL);

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
      Node * value = base->getPropertyValue(name);
      if (value == NULL) {
        diag::error(name->location()) << "Undefined symbol: " << name;
        return &Node::UNDEFINED_NODE;
      }
      // If it's a lazy property, then evaluate it now.
      if (base->nodeKind() == Node::NK_OBJECT) {
        Object * obj = static_cast<Object *>(base);
        if (obj->definition() != NULL) {
          evalObjectContents(obj);
        }
        Property * propDef = obj->findProperty(name);
        if (propDef != NULL && propDef->lazy()) {
          value = eval(value);
        }
      }
      return value;
    }

    case Node::NK_GET_ELEMENT: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
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
        return new Literal<double>(Node::NK_FLOAT, n->location(), a0->type(), v0 + v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return new Literal<int>(Node::NK_INTEGER, n->location(), a0->type(), v0 + v1);
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
        return new Literal<double>(Node::NK_FLOAT, n->location(), a0->type(), v0 - v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return new Literal<int>(Node::NK_INTEGER, n->location(), a0->type(), v0 - v1);
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
        return new Literal<double>(Node::NK_FLOAT, n->location(), a0->type(), v0 * v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return new Literal<int>(Node::NK_INTEGER, n->location(), a0->type(), v0 * v1);
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
        return new Literal<double>(Node::NK_FLOAT, n->location(), a0->type(), v0 / v1);
      } else if (a0->nodeKind() == Node::NK_INTEGER) {
        M_ASSERT(a1->nodeKind() == Node::NK_INTEGER);
        int v0 = static_cast<const Literal<int> *>(a0)->value();
        int v1 = static_cast<const Literal<int> *>(a1)->value();
        return new Literal<int>(Node::NK_INTEGER, n->location(), a0->type(), v0 / v1);
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
        return new Literal<int>(Node::NK_INTEGER, n->location(), a0->type(), v0 % v1);
      } else {
        diag::error(a0->location()) << "Not a number: '" << a0 << "'.";
        return &Node::UNDEFINED_NODE;
      }
    }

    case Node::NK_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      M_ASSERT(false) << "Implement";
      break;
    }

    case Node::NK_NOT_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      M_ASSERT(false) << "Implement";
      break;
    }

    case Node::NK_LESS: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      M_ASSERT(false) << "Implement";
      break;
    }

    case Node::NK_LESS_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      M_ASSERT(false) << "Implement";
      break;
    }

    case Node::NK_GREATER: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      M_ASSERT(false) << "Implement";
      break;
    }

    case Node::NK_GREATER_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      M_ASSERT(false) << "Implement";
      break;
    }

    case Node::NK_AND: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      break;
    }

    case Node::NK_OR: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0));
      Node * a1 = eval(op->arg(1));
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      break;
    }

    case Node::NK_MAKE_MODULE:
    case Node::NK_MODULE:
    case Node::NK_PROJECT: {
      console::err() << "Expression cannot be evaluated: " << n->nodeKind();
      return NULL;
    }

    default: {
      console::err() << "Undefined node type: " << unsigned(n->nodeKind());
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
  Fundamentals * fundamentals = _module->project()->fundamentals();
  Object * obj = new Object(
      Node::NK_OPTION,
      op->location(),
      static_cast<Object *>(fundamentals->option.ptr()));
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
  StringDict<Node>::const_iterator prev = _module->properties().find(propName);
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
  Ref<Oper> definition = static_cast<Oper *>(obj->definition());
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
        Property * propDef = obj->findProperty(static_cast<String *>(propName));
        if (propDef != NULL) {
          diag::error(propName->location()) << "Property '" << propName
              << "' is already defined on object '" << obj->nameSafe() << "'.";
          diag::info(propDef->location()) << "Previous property definition.";
        }
        String * name = static_cast<String *>(propName);
        Type * type = evalTypeExpression(op->arg(1));
        Node * value = op->arg(2);
        Literal<int> * flags = static_cast<Literal<int> *>(op->arg(3));
        bool lazy = flags->value() != 0;
        if (!lazy) {
          value = eval(value);
        }
        if (type == NULL) {
          type = value->type();
          M_ASSERT(type != NULL);
        }
        Property * prop = new Property(value, type, lazy);
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

Type * Evaluator::evalTypeExpression(Node * ty) {
  if (ty == NULL) {
    return NULL;
  } else if (ty->nodeKind() == Node::NK_TYPENAME) {
    return static_cast<Type *>(ty);
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
      return typeRegistry().getListType(param);
    } else if (base == TypeRegistry::genericDictType()) {
      M_ASSERT(false) << "Implement";
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
  while (src < srcEnd) {
    Node * n = eval(*src++);
    *dst++ = n;
  }

  Oper * result = Oper::create(Node::NK_LIST, op->location(), op->type(), args);
  result->setType(op->type());
  return result;
}

Node * Evaluator::evalDict(Oper * op) {
  SmallVector<Node *, 32> args;
  args.reserve(op->size());
  evalArgs(op->args().begin(), args.begin(), op->size());
  return NULL;
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
      func = s->getPropertyValue(name);
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
    func = self->getPropertyValue(name);
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
    return (*fn->handler())(this, fn, self, NodeArray(args));
  } else {
    diag::error(callable->location()) << "Expression is not a function: '" << callable << "'.";
    return &Node::UNDEFINED_NODE;
  }
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
  if (name != NULL) {
    obj->setName(name);
  }
  return obj;
}

bool Evaluator::setObjectProperty(Object * obj, String * propName, Node * propValue) {
  String * propNameStr = static_cast<String *>(propName);
  Property * propDef = obj->findProperty(propNameStr);
  if (propDef == NULL) {
    // Ummm, we might need to evaluate the protos!
//    for (Object * proto = obj->prototype(); proto != NULL; proto = proto->prototype()) {
//      if (proto->definition() != NULL) {
//        evalObjectContents(proto);
//      }
//    }
//    propDef = obj->findProperty(propNameStr);
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

  /// TODO: Check type compatibility.
  /// TODO: Check laziness.
  if (propValue->nodeKind() == Node::NK_MAKE_OBJECT) {
    propValue = makeObject(static_cast<Oper *>(propValue), propNameStr);
  } else if (!propDef->lazy()) {
    propValue = eval(propValue);
  }
  if (propValue == NULL) {
    return false;
  }
  obj->properties()[propNameStr] = propValue;
  return true;
}

void Evaluator::evalArgs(NodeArray::iterator src, Node ** dst, size_t count) {
  while (count--) {
    *dst++ = eval(*src++);
  }
}

Node * Evaluator::coerce(Node * n, Type * ty) {
  M_ASSERT(ty != NULL);
  M_ASSERT(n->type() != NULL) << "Node '" << n << "' has no type information!";
  if (n->type() == ty) {
    return n;
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
        return new Literal<int>(
            Node::NK_INTEGER, n->location(), TypeRegistry::integerType(), int(v0));
      }
      break;
    }
    case Type::FLOAT: {
      if (n->nodeKind() == Node::NK_INTEGER) {
        int v0 = static_cast<const Literal<int> *>(n)->value();
        return new Literal<double>(
            Node::NK_FLOAT, n->location(), TypeRegistry::floatType(), v0);
      }
      break;
    }
    case Type::STRING:
      break;
    case Type::LIST:
      break;
    case Type::DICTIONARY:
      break;
    case Type::OBJECT:
      break;
    case Type::FUNCTION:
      break;
    default:
      diag::error(n->location()) << "Invalid type for coercion: " << ty->typeKind();
      return n;
  }

  diag::error(n->location()) << "Cannot coerce value of type " << srcType << " to " << ty;
  return n;
}

TypeRegistry & Evaluator::typeRegistry() const {
  Fundamentals * fundamentals = _module->project()->fundamentals();
  M_ASSERT(fundamentals != NULL);
  return fundamentals->typeRegistry();
}

}
