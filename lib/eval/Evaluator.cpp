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

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

using namespace mint::strings;

static inline int cmp(int lhs, int rhs) {
  return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
}

static inline int cmp(double lhs, double rhs) {
  return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
}

Evaluator::Evaluator(Node * startingScope)
  : _typeRegistry(TypeRegistry::get())
  , _lexicalScope(startingScope)
  , _self(NULL)
  , _caller(NULL)
{}

Evaluator::Evaluator(Evaluator & parent)
  : _typeRegistry(TypeRegistry::get())
  , _lexicalScope(parent._lexicalScope)
  , _self(parent._self)
  , _caller(&parent)
{}

Node * Evaluator::eval(Node * n, Type * expected) {
  switch (n->nodeKind()) {
    case Node::NK_UNDEFINED:
    case Node::NK_BOOL:
    case Node::NK_INTEGER:
    case Node::NK_FLOAT:
    case Node::NK_STRING:
    case Node::NK_LIST:
    case Node::NK_DICT:
    case Node::NK_OBJECT:
    case Node::NK_MODULE:
    case Node::NK_FUNCTION:
    case Node::NK_TYPENAME:
      return n;

    case Node::NK_IDENT: {
      M_ASSERT(_lexicalScope != NULL);
      String * ident = static_cast<String *>(n);
      AttributeLookup lookup;
      Node * searchScope = lookupIdent(*ident, lookup);
      if (searchScope != NULL) {
        return evalAttribute(ident->location(), lookup, searchScope, *ident);
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
      return _self;

    case Node::NK_SUPER:
      break;

    // Operations
    case Node::NK_GET_MEMBER: {
      Oper * op = static_cast<Oper *>(n);
      Node * base = eval(op->arg(0), NULL);
      if (base->isUndefined()) {
        return &Node::UNDEFINED_NODE;
      }
      String * name = String::dyn_cast(op->arg(1));
      if (name == NULL) {
        diag::error(op->arg(1)->location()) << "Invalid node type for object member: " << op;
        return &Node::UNDEFINED_NODE;
      }
      if (base->nodeKind() == Node::NK_OBJECT) {
        Object * baseObj = static_cast<Object *>(base);
        if (!ensureObjectContents(baseObj)) {
          return &Node::UNDEFINED_NODE;
        }
      }
      AttributeLookup lookup;
      if (!base->getAttribute(*name, lookup)) {
        diag::error(name->location()) << "Undefined symbol for object '" << base << "': " << name;
        return &Node::UNDEFINED_NODE;
      }
      return evalAttribute(name->location(), lookup, base, *name);
    }

    case Node::NK_GET_ELEMENT: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0), NULL);
      Node * a1 = eval(op->arg(1), NULL);
      M_ASSERT(a0 != NULL);
      M_ASSERT(a1 != NULL);
      if (a0->nodeKind() == Node::NK_DICT || a0->nodeKind() == Node::NK_MODULE || a0->nodeKind() == Node::NK_OBJECT) {
        String * key = String::dyn_cast(a1);
        if (key == NULL) {
          diag::error(a1->location()) << "Invalid key type: " << a1->nodeKind();
        }
        AttributeLookup lookup;
        if (!a0->getAttribute(*key, lookup)) {
          return &Node::UNDEFINED_NODE;
        }
        return evalAttribute(key->location(), lookup, a0, *key);
      } else {
        Node * result = a0->getElement(a1);
        if (result == NULL) {
          return &Node::UNDEFINED_NODE;
        }
        return result;
      }
      break;
    }

    case Node::NK_CALL:
      return evalCall(static_cast<Oper *>(n));

    // Unary
    case Node::NK_NEGATE: {
      Oper * op = static_cast<Oper *>(n);
      Node * n = eval(op->arg(0), expected);
      M_ASSERT(n != NULL);
      break;
    }

    // Binary
    case Node::NK_ADD: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0), expected);
      Node * a1 = eval(op->arg(1), expected);
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
      Node * a0 = eval(op->arg(0), expected);
      Node * a1 = eval(op->arg(1), expected);
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
      Node * a0 = eval(op->arg(0), expected);
      Node * a1 = eval(op->arg(1), expected);
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
      Node * a0 = eval(op->arg(0), expected);
      Node * a1 = eval(op->arg(1), expected);
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
      Node * a0 = eval(op->arg(0), expected);
      Node * a1 = eval(op->arg(1), expected);
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
      return Node::makeBool(equal(op->location(), eval(op->arg(0), NULL), eval(op->arg(1), NULL)));
    }

    case Node::NK_NOT_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(!equal(op->location(), eval(op->arg(0), NULL), eval(op->arg(1), NULL)));
    }

    case Node::NK_LESS: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0), NULL), eval(op->arg(1), NULL)) < 0);
    }

    case Node::NK_LESS_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0), NULL), eval(op->arg(1), NULL)) <= 0);
    }

    case Node::NK_GREATER: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0), NULL), eval(op->arg(1), NULL)) > 0);
    }

    case Node::NK_GREATER_EQUAL: {
      Oper * op = static_cast<Oper *>(n);
      return Node::makeBool(compare(op->location(), eval(op->arg(0), NULL), eval(op->arg(1), NULL)) >= 0);
    }

    case Node::NK_AND: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0), expected);
      if (!isNonNil(a0)) {
        return a0;
      }
      return eval(op->arg(1), expected);
    }

    case Node::NK_OR: {
      Oper * op = static_cast<Oper *>(n);
      Node * a0 = eval(op->arg(0), expected);
      if (isNonNil(a0)) {
        return a0;
      }
      return eval(op->arg(1), expected);
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
      func->setParentScope(_lexicalScope);

      Node * closureArgs[] = { func, _lexicalScope, _self };
      Oper * closure = Oper::create(Node::NK_CLOSURE, op->location(), fnType, closureArgs);
      return closure;
    }

    case Node::NK_DEFERRED: {
      Oper * op = static_cast<Oper *>(n);
      M_ASSERT(op->size() == 2);
      Node * result = call(op->location(), n, _self, NodeArray());
      M_ASSERT(result != NULL);
      Node * coercedResult = coerce(result, op->type());
      if (coercedResult == NULL) {
        return &Node::UNDEFINED_NODE;
      }
      return coercedResult;
    }

    case Node::NK_CONCAT:
      return evalConcat(static_cast<Oper *>(n), expected);

    case Node::NK_DO:
      return evalDoStmt(static_cast<Oper *>(n));

    case Node::NK_LET:
      return evalLetStmt(static_cast<Oper *>(n));

    case Node::NK_IF: {
      Oper * op = static_cast<Oper *>(n);
      Node * test = eval(op->arg(0), TypeRegistry::boolType());
      M_ASSERT(test != NULL);
      if (isNonNil(test)) {
        return eval(op->arg(1), expected);
      } else {
        return eval(op->arg(2), expected);
      }
    }

    case Node::NK_MAKE_MODULE:
    case Node::NK_PROJECT: {
      console::err() << "Expression cannot be evaluated: " << n->nodeKind();
      return NULL;
    }

    default: {
      if (n->nodeKind() <= Node::NK_IMPORT_ALL) {
        M_ASSERT(false) << "Unhandled node type: " << n->nodeKind();
      } else {
        M_ASSERT(false) << "Undefined node type: " << unsigned(n->nodeKind());
      }
      return NULL;
    }
  }

  M_ASSERT(false) << "Invalid state for nodeKind " << n->nodeKind();
  return NULL;
}

bool Evaluator::evalModuleContents(Module * module, Oper * content) {
  NodeArray::const_iterator it = content->begin(), itEnd = content->end();
  Node * savedScope = setLexicalScope(module);
  while (it != itEnd) {
    Node * n = *it++;
    switch (n->nodeKind()) {
      case Node::NK_SET_MEMBER: {
        if (!evalModuleAttribute(module, static_cast<Oper *>(n))) {
          setLexicalScope(savedScope);
          return false;
        }
        break;
      }

      case Node::NK_MAKE_ACTION: {
        Oper * action = static_cast<Oper *>(n);
        M_ASSERT(action->size() == 1);
        Node * value = eval(action->arg(0), NULL);
        if (value != NULL) {
          module->addAction(value);
        }
        break;
      }

      case Node::NK_IMPORT: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() == 1);
        Module * m = importModule(module, importOp->arg(0));
        if (m != NULL) {
          //console::out() << "New module: " << m->name() << "\n";
          //console::err() << "New module: " << m->name() << "\n";

          module->addImportScope(m);
          // This one is tricky - we want to preserve the entire relative path
          // between the two modules.
          //M_ASSERT(false) << "implement";
        }
        break;
      }

      case Node::NK_IMPORT_AS: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() == 2);
        Module * m = importModule(module, importOp->arg(0));
        if (m != NULL) {
          /// Create a scope to store the module by the 'as' name.
          Object * newScope = new Object(Node::NK_DICT, importOp->location(), NULL);
          String * asName = String::cast(importOp->arg(1));
          newScope->attrs()[asName] = m;
          module->addImportScope(newScope);
        }
        break;
      }

      case Node::NK_IMPORT_FROM: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() > 1);
        Module * m = importModule(module, importOp->arg(0));
        if (m != NULL) {
          // We're not importing the entire module, so create a special import scope
          // to hold just the symbols we want.
          Object * newScope = new Object(Node::NK_DICT, importOp->location(), NULL);
          for (Oper::const_iterator it = importOp->begin() + 1, itEnd = importOp->end();
              it != itEnd; ++it) {
            String * symName = String::cast(*it);
            Node * value = m->getAttributeValue(*symName);
            if (value == NULL) {
              diag::error(symName->location()) << "Undefined symbol '" << symName << "'.";
              return NULL;
            }
            newScope->attrs()[symName] = value;
          }
          module->addImportScope(newScope);
        }
        break;
      }

      case Node::NK_IMPORT_ALL: {
        Oper * importOp = static_cast<Oper *>(n);
        M_ASSERT(importOp != NULL && importOp->size() == 1);
        Module * m = importModule(module, importOp->arg(0));
        if (m != NULL) {
          module->addImportScope(m);
        }
        break;
      }

      case Node::NK_IF: {
        Oper * op = static_cast<Oper *>(n);
        Node * test = eval(op->arg(0), TypeRegistry::boolType());
        M_ASSERT(test != NULL);
        if (isNonNil(test)) {
          evalModuleContents(module, op->arg(1)->requireOper());
        } else if (op->size() == 3) {
          evalModuleContents(module, op->arg(2)->requireOper());
        }
        break;
      }

      default:
        diag::error(n->location()) << "Invalid expression for module attribute: '" << n << "'.";
        setLexicalScope(savedScope);
        return false;
    }
  }

  setLexicalScope(savedScope);
  return true;
}

bool Evaluator::evalModuleAttribute(Module * module, Oper * op) {
  Node * attrName = op->arg(0);
  if (attrName->nodeKind() == Node::NK_IDENT) {
    String * ident = static_cast<String *>(attrName);
    if (checkAlreadyDefined(ident->location(), module, *ident)) {
      return false;
    }
    Node * attrValue = op->arg(1);
    if (attrValue->nodeKind() == Node::NK_MAKE_OBJECT) {
      attrValue = makeObject(static_cast<Oper *>(attrValue), ident);
    } else {
      attrValue = eval(attrValue, NULL);
    }
    module->setAttribute(ident, attrValue);
  } else {
    diag::error(op->location()) << "Invalid expression for module attribute name: '"
        << attrName << "'.";
    return false;
  }
  return true;
}

bool Evaluator::setConfigVar(Node * config) {
  M_ASSERT(config->nodeKind() == Node::NK_SET_MEMBER);
  Oper * op = static_cast<Oper *>(config);
  Node * lval = op->arg(0);
  Node * value = op->arg(1);
  Node * scope = _lexicalScope;
  String * name = NULL;
  switch (lval->nodeKind()) {
    case Node::NK_STRING:
    case Node::NK_IDENT:
      name = static_cast<String *>(lval);
      break;

    case Node::NK_GET_MEMBER: {
      Oper * getMemberOp = static_cast<Oper *>(lval);
      scope = eval(getMemberOp->arg(0), NULL);
      if (scope == NULL) {
        diag::error(config->location()) << "Not found: " << getMemberOp;
        return false;
      }
      name = String::cast(getMemberOp->arg(1));
      break;
    }

    default:
      diag::error(config->location()) << "Invalid configuration variable '" << lval << "'.";
      return false;
  }

  Object * obj = scope->asObject();
  if (obj == NULL) {
    diag::error(config->location()) << "Invalid configuration variable '" << lval << "'.";
    return false;
  }

  obj->attrs()[name] = value;
  return true;
}

bool Evaluator::ensureObjectContents(Object * obj) {
  if (obj->definition() != NULL) {
    return evalObjectContents(obj);
  }
  return true;
}

bool Evaluator::evalObjectContents(Object * obj) {
  Node * savedSelf = setSelf(obj);
  Node * savedScope = setLexicalScope(obj->parentScope());
  Oper * definition = static_cast<Oper *>(obj->definition());
  obj->setDefinition(NULL);
  if (!ensureObjectContents(obj->prototype())) {
    return false;
  }
  bool success = true;
  M_ASSERT(definition->size() >= 1);
  for (Oper::const_iterator
      it = definition->begin() + 1, itEnd = definition->end(); it != itEnd; ++it) {
    Node * n = *it;
    switch (n->nodeKind()) {
      case Node::NK_SET_MEMBER: {
        Oper * op = static_cast<Oper *>(n);
        Node * attrName = op->arg(0);
        Node * attrValue = op->arg(1);
        if (attrName->nodeKind() != Node::NK_IDENT) {
          diag::error(attrName->location()) << "Invalid expression for object attribute name '"
              << attrName << "'.";
          success = false;
          continue;
        }
        setAttribute(obj, static_cast<String *>(attrName), attrValue);
        break;
      }

      case Node::NK_MAKE_PARAM: {
        Oper * op = static_cast<Oper *>(n);
        Node * attrName = op->arg(0);
        if (attrName->nodeKind() != Node::NK_IDENT) {
          diag::error(attrName->location()) << "Invalid expression for object attribute name '"
              << attrName << "'.";
          success = false;
          continue;
        }
        String * name = static_cast<String *>(attrName);
        if (checkAlreadyDefined(attrName->location(), obj, *name)) {
          goto done;
        }
        Type * type = evalTypeExpression(op->arg(1));
        Node * value = op->arg(2);
        Literal<int> * flags = static_cast<Literal<int> *>(op->arg(3));
        if (value->nodeKind() == Node::NK_MAKE_DEFERRED) {
          value = createDeferred(static_cast<Oper *>(value), type);
        } else {
          value = eval(value, type);
          M_ASSERT(value != NULL) << "Evaluation of " << op->arg(2) << " returned NULL";
        }
        if (type == NULL) {
          type = value->type();
          M_ASSERT(type != NULL);
        }
        AttributeDefinition * prop = new AttributeDefinition(value, type, flags->value());
        obj->attrs()[name] = prop;
        break;
      }

      default:
        diag::error(n->location()) << "Invalid expression for object attribute: '" << n << "'.";
        success = false;
        goto done;
    }
  }

done:
  _self = savedSelf;
  setLexicalScope(savedScope);
  return success;
}

Type * Evaluator::evalTypeExpression(Node * ty) {
  if (ty == NULL) {
    return NULL;
  } else if (ty->nodeKind() == Node::NK_TYPENAME || ty->nodeKind() == Node::NK_OBJECT) {
    return static_cast<Type *>(ty);
  } else if (ty->nodeKind() == Node::NK_IDENT) {
    Node * n = eval(ty, NULL);
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
    Node * n = eval(*src++, NULL);
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
  Object * result = new Object(Node::NK_DICT, op->location(), TypeRegistry::objectType());
  for (Oper::const_iterator it = args.begin(), itEnd = args.end(); it != itEnd;) {
    Node * key = *it++;
    Node * value = *it++;
    String * strKey = String::dyn_cast(key);
    if (strKey == NULL) {
      diag::error(key->location()) << "Only string keys are supported for dictionary types";
      continue;
    }
    result->attrs()[strKey] = value;
  }
  return result;
}

Node * Evaluator::evalCall(Oper * op) {
  // First arg is handled differently
  Node * callable = op->arg(0);
  Node * func = NULL;
  Node * selfArg = _self;
  Node * lexScope = _lexicalScope;
  // TODO: Use AttributeLookup here?
  if (callable->nodeKind() == Node::NK_IDENT) {
    M_ASSERT(_lexicalScope != NULL);
    String * name = static_cast<String *>(callable);
    AttributeLookup lookup;
    selfArg = lookupIdent(*name, lookup);
    if (selfArg == NULL) {
      diag::error(callable->location()) << "Function not found: '" << name << "'.";
      return &Node::UNDEFINED_NODE;
    }
    func = lookup.value;
    lexScope = lookup.foundScope->parentScope();
  } else if (callable->nodeKind() == Node::NK_GET_MEMBER) {
    Oper * getMemberOp = static_cast<Oper *>(callable);
    selfArg = eval(getMemberOp->arg(0), NULL);
    if (selfArg->nodeKind() == Node::NK_UNDEFINED) {
      return &Node::UNDEFINED_NODE;
    }
    String * name = static_cast<String *>(getMemberOp->arg(1));
    if (selfArg->nodeKind() == Node::NK_LIST) {
      func = TypeRegistry::listType()->getAttributeValue(*name);
    } else {
      AttributeLookup lookup;
      if (selfArg->getAttribute(*name, lookup)) {
        func = lookup.value;
        lexScope = lookup.foundScope;
        M_ASSERT(func != NULL);
      }
    }
    if (!func) {
      diag::error(callable->location()) << "Method not found: '" << name << "'.";
      return &Node::UNDEFINED_NODE;
    }
  } else {
    func = eval(callable, NULL);
    if (func->nodeKind() == Node::NK_UNDEFINED) {
      return &Node::UNDEFINED_NODE;
    }
  }

  Function * fn = NULL;
  if (func->nodeKind() == Node::NK_CLOSURE) {
    fn = static_cast<Function *>(static_cast<Oper *>(func)->arg(0));
  } else if (func->nodeKind() == Node::NK_FUNCTION) {
    fn = static_cast<Function *>(func);
  } else {
    diag::error(callable->location()) << "Expression is not callable: '" << callable << "'.";
    return &Node::UNDEFINED_NODE;
  }

  size_t argCount = op->size() - 1;
  SmallVector<Node *, 32> args;
  args.resize(argCount);

  if (fn->argCount() != argCount) {
    diag::error(op->location()) << "Function expected " << fn->argCount()
        << " arguments, but was passed " << argCount;
    return &Node::UNDEFINED_NODE;
  }

  for (size_t i = 0; i < argCount; ++i) {
    Type * argType = fn->argType(i);
    Node * arg = op->arg(i + 1);
    Node * coercedArg = coerce(eval(arg, argType), argType);
    if (coercedArg == NULL) {
      return &Node::UNDEFINED_NODE;
    }
    args[i] = coercedArg;
  }

  return call(op->location(), func, selfArg, NodeArray(args));
}

Node * Evaluator::call(Location loc, Node * callable, Node * selfArg, NodeArray args) {
  Evaluator nested(*this);
  if (callable->nodeKind() == Node::NK_CLOSURE) {
    Oper * closureOp = static_cast<Oper *>(callable);
    callable = eval(closureOp->arg(0), NULL);
    nested._lexicalScope = closureOp->arg(1);
    nested._self = closureOp->arg(2);
  } else if (callable->nodeKind() == Node::NK_DEFERRED) {
    Oper * deferredOp = static_cast<Oper *>(callable);
    callable = deferredOp->arg(0);
    nested._lexicalScope = deferredOp->arg(1);
  }

  if (callable->nodeKind() == Node::NK_FUNCTION) {
    Function * fn = static_cast<Function *>(callable);
    Node * result = (*fn->handler())(loc, &nested, fn, selfArg, NodeArray(args));
    M_ASSERT(result != NULL) << "NULL returned from function call";
    return result;
  } else {
    diag::error(callable->location()) << "Expression is not callable: '" << callable << "'.";
    return &Node::UNDEFINED_NODE;
  }
}

Node * Evaluator::evalConcat(Oper * op, Type * expected) {
  SmallVector<Node *, 32> args;
  args.resize(op->size());
  unsigned numStringArgs = 0;
  Type * listType = NULL;
  SmallVectorImpl<Node *>::iterator out = args.begin();
  for (Oper::const_iterator it = op->begin(), itEnd = op->end(); it != itEnd; ++it) {
    Node * n = *it;
    Node * arg = eval(n, expected);
    if (arg->nodeKind() == Node::NK_ATTRDEF) {
      arg = eval(n, expected);
    }
    M_ASSERT(arg->nodeKind() != Node::NK_ATTRDEF);
    if (arg->type() != NULL) {
      if (arg->type()->typeKind() == Type::STRING) {
        ++numStringArgs;
      } else if (arg->type()->typeKind() == Type::LIST) {
        listType = selectCommonType(listType, arg->type());
      }
    }
    *out++ = arg;
  }

  bool isStringResult = false;
  if (expected != NULL) {
    if (expected->typeKind() == Type::STRING) {
      isStringResult = true;
    }
  }

  if (listType != 0 && !isStringResult) {
    // List concatenation
    SmallVector<Node *, 32> result;
    size_t index = 0;
    for (Oper::iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it, ++index) {
      Node * n = *it;
      if (n->isUndefined()) {
        // Skip undefined
        continue;
      }
      Node * coercedValue = coerce(n, listType);
      if (coercedValue == NULL || coercedValue->nodeKind() != Node::NK_LIST) {
        Node * arg = op->arg(index);
        diag::error(arg->location()) << "Attempt to concatenate non-list type: " << n->type();
        diag::info(op->location()) << "In this expression: " << op;
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
    result = eval(*it, NULL);
  }
  // Return the value of the last evaluated node.
  return result;
}

Node * Evaluator::evalLetStmt(Oper * op) {
  Node * result = &Node::UNDEFINED_NODE;
  Object * localScope = new Object(Node::NK_DICT, op->location(), NULL);
  localScope->setParentScope(_lexicalScope);
  Node * savedScope = setLexicalScope(localScope);
  M_ASSERT(op->size() > 1);
  // Set up the local environment.
  for (Oper::const_iterator it = op->begin(), itEnd = op->end() - 1; it != itEnd; ++it) {
    Oper * setOp = static_cast<Oper *>(*it);
    M_ASSERT(setOp->nodeKind() == Node::NK_SET_MEMBER);
    String * attrName = String::cast(setOp->arg(0));
    Node * attrValue = eval(setOp->arg(1), NULL);
    localScope->attrs()[attrName] = attrValue;
  }
  result = eval(*(op->end() - 1), NULL);
  setLexicalScope(savedScope);
  return result;
}

Node * Evaluator::makeObject(Oper * op, String * name) {
  M_ASSERT(op->size() >= 1);
  Node * protoExpr = op->arg(0);
  Node * prototype = eval(protoExpr, NULL);
  if (prototype->nodeKind() == Node::NK_UNDEFINED) {
    return &Node::UNDEFINED_NODE;
  } else if (prototype->nodeKind() != Node::NK_OBJECT) {
    diag::error(protoExpr->location()) << "Prototype expression is not an object: '"
        << protoExpr << "'.";
    return &Node::UNDEFINED_NODE;
  }

  Object * obj = new Object(op->location(), static_cast<Object *>(prototype), op);
  obj->setParentScope(_lexicalScope);
  //obj->setType(obj);
  if (name != NULL) {
    obj->setName(name);
  }
  return obj;
}

bool Evaluator::setAttribute(Object * obj, String * attrName, Node * attrValue) {
  AttributeLookup lookup;
  if (!obj->getAttribute(*attrName, lookup)) {
    diag::error(attrName->location()) << "Attempt to set non-existent attribute '"
        << attrName << "' on object '" << obj->nameSafe() << "'.";
    return false;
  }
  AttributeDefinition * attrDef = lookup.definition;

  Attributes::const_iterator pi = obj->attrs().find(attrName);
  if (pi != obj->attrs().end()) {
    diag::error(attrName->location()) << "Property '" << attrName
        << "' has already be defined on object '" << obj->nameSafe() << "'.";
    return false;
  }

  if (attrValue->nodeKind() == Node::NK_MAKE_OBJECT) {
    attrValue = makeObject(static_cast<Oper *>(attrValue), attrName);
  } else if (attrValue->nodeKind() == Node::NK_MAKE_DEFERRED) {
    attrValue = createDeferred(static_cast<Oper *>(attrValue), attrDef->type());
    obj->attrs()[attrName] = attrValue;
    return true;
  } else {
    attrValue = eval(attrValue, attrDef->type());
  }
  if (attrValue == NULL) {
    return false;
  }
  if (attrDef->type() != NULL && attrValue->nodeKind() != Node::NK_UNDEFINED) {
    Node * coercedValue = coerce(attrValue, attrDef->type());
    if (coercedValue == NULL) {
      return false;
    }
    attrValue = coercedValue;
  }

  obj->attrs()[attrName] = attrValue;
  return true;
}

Node * Evaluator::createDeferred(Oper * deferred, Type * type) {
  DerivedType * fnType = _typeRegistry.getFunctionType(type, TypeArray());
  Function * func = new Function(
      Node::NK_FUNCTION, deferred->location(), fnType, evalFunctionBody);
  func->setBody(deferred->arg(0));
  func->setParentScope(_lexicalScope);
  Node * closureArgs[] = { func, _lexicalScope };
  return Oper::create(Node::NK_DEFERRED, deferred->location(), type, closureArgs);
}

Node * Evaluator::attributeValue(Node * searchScope, StringRef name) {
  AttributeLookup lookup;
  if (!searchScope->getAttribute(name, lookup)) {
    return NULL;
  }
  if (lookup.definition != NULL && lookup.value->nodeKind() == Node::NK_DEFERRED) {
    Evaluator nested(*this);
    nested._self = searchScope;
    nested._lexicalScope = lookup.foundScope->parentScope();
    lookup.value = nested.eval(lookup.value, lookup.definition->type());
  }
  return lookup.value;
}

Oper * Evaluator::attributeValueAsList(Node * searchScope, StringRef name) {
  Node * result = attributeValue(searchScope, name);
  if (result == NULL) {
    return NULL;
  } else if (result->nodeKind() != Node::NK_LIST) {
    diag::error(result->location()) << "Expected attribute '" << name << "' to be a list.";
    return NULL;
  } else {
    return static_cast<Oper *>(result);
  }
}

Node * Evaluator::optionValue(Object * obj) {
  M_ASSERT(obj->inheritsFrom(TypeRegistry::optionType()));
  M_ASSERT(obj->name() != NULL);
  Module * m = obj->module();
  M_ASSERT(m != NULL);
  Project * p = m->project();
  M_ASSERT(p != NULL);
  return p->optionValue(obj->name()->value());
}

Node * Evaluator::evalAttribute(
    Location loc, AttributeLookup & propLookup, Node * searchScope, StringRef name) {
  if (propLookup.definition != NULL && propLookup.value->nodeKind() == Node::NK_DEFERRED) {
    Evaluator nested(*this);
    nested._self = searchScope;
    nested._lexicalScope = propLookup.foundScope->parentScope();
    propLookup.value = nested.eval(propLookup.value, propLookup.definition->type());
  }
  return propLookup.value;
}

Node * Evaluator::lookupIdent(StringRef name, AttributeLookup & lookup) {
  // First try search 'self' and inheritors
  if (_self != NULL && _self->getAttribute(name, lookup)) {
    return _self;
  }
  // Then try searching the current scope and ancestors.
  for (Node * s = _lexicalScope; s != NULL; s = s->parentScope()) {
    if (s->getAttribute(name, lookup)) {
      return s;
    }
  }
  return NULL;
}

void Evaluator::evalArgs(NodeArray::iterator src, Node ** dst, size_t count) {
  while (count--) {
    *dst++ = eval(*src++, NULL);
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
      return dict->attrs().size() != 0;
    }

    case Node::NK_OBJECT: {
      // For now the name 'value' is magic.
      Object * obj = static_cast<Object *>(n);
      if (obj->inheritsFrom(TypeRegistry::optionType())) {
        Node * val = optionValue(obj);
        if (val != NULL) {
          return isNonNil(val);
        }
      } else {
        Node * value = attributeValue(obj, "value");
        if (value != NULL) {
          return isNonNil(value);
        }
      }
      return true;
    }

    default:
      return true;
  }
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
  Type * srcType = n->type();
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

        case Node::NK_OBJECT: {
          Object * obj = static_cast<Object *>(n);
          if (obj->inheritsFrom(TypeRegistry::optionType())) {
            Node * val = optionValue(obj);
            if (val != NULL) {
              return coerce(val, ty);
            }
          }
          break;
        }

        default:
          break;
      }
      break;
    }

    case Type::LIST: {
      DerivedType * listType = static_cast<DerivedType *>(ty);
      Type * elementType = listType->params()[0];
      if (elementType->typeKind() == Type::ANY) {
        return n;
      }
      if (n->nodeKind() == Node::NK_LIST) {
        Oper * list = static_cast<Oper *>(n);
        SmallVector<Node *, 32> elements;
        elements.resize(list->size());
        SmallVector<Node *, 32>::iterator put = elements.begin();
        bool changed = false;
        for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
          Node * el = coerce(*it, elementType);
          if (el == NULL) {
            el = *it;
          }
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
      if (n->nodeKind() >= Node::NK_OBJECTS_FIRST && n->nodeKind() <= Node::NK_OBJECTS_LAST) {
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

Node * Evaluator::evalFunctionBody(Location loc, Evaluator * ex, Function * fn, Node * self,
    NodeArray args) {
  Node * savedLexical = ex->_lexicalScope;
  if (fn->argCount() > 0) {
    Object * localScope = new Object(fn->location(), NULL, NULL);
    localScope->setParentScope(ex->_lexicalScope);
    M_ASSERT(args.size() == fn->argCount());
    SmallVectorImpl<Parameter>::const_iterator pi = fn->params().begin();
    for (NodeArray::const_iterator it = args.begin(), itEnd = args.end(); it != itEnd; ++it, ++pi) {
      const Parameter & param = *pi;
      localScope->attrs()[param.name()] = *it;
    }
    ex->_lexicalScope = localScope;
  }
  Node * result = ex->eval(fn->body(), NULL);
  ex->setLexicalScope(savedLexical);
  return result;
}

Module * Evaluator::importModule(Module * importingModule, Node * path) {
  M_ASSERT(path != NULL);
  M_ASSERT(importingModule != NULL);
  M_ASSERT(importingModule->project() != NULL);
  StringRef pathStr = String::cast(path)->value();

  // The project from which we are going to load from.
  Project * project = importingModule->project();
  Module * result = NULL;

  size_t dotPos = pathStr.find('.');
  size_t colonPos = pathStr.find(':');
  if (colonPos < dotPos) {
    // There's a colon before the first dot
    StringRef projName = pathStr.substr(0, colonPos);
    project = importingModule->project()->buildConfig()->getProject(projName);
    if (project == NULL) {
      diag::error(path->location()) << "Project '" << projName << "' not found.";
      return NULL;
    }
    pathStr = pathStr.substr(colonPos + 1);
    result = project->loadModule(pathStr);
  } else if (importingModule->name()->value().empty()) {
    result = project->loadModule(pathStr);
  } else {
    SmallString<64> combinedPath(importingModule->name()->value());
    dotPos = StringRef(combinedPath).rfind('.');
    if (dotPos != StringRef::npos) {
      combinedPath.erase(combinedPath.begin() + dotPos + 1, combinedPath.end());
    }
    combinedPath.append(pathStr.begin(), pathStr.end());
    result = project->loadModule(combinedPath);
    if (result == NULL) {
      result = project->loadModule(pathStr);
    }
  }

  if (result == NULL) {
    diag::error(path->location()) << "Module '" << pathStr << "' not found.";
    exit(1);
  }

  return result;
}

bool Evaluator::checkAlreadyDefined(Location loc, Node * scope, StringRef name) {
  AttributeLookup lookup;
  if (scope->getAttribute(name, lookup) && lookup.foundScope == scope) {
    diag::error(loc) << "Attribute '" << name << "' is already defined in this scope";
    diag::info(lookup.value->location()) << "at this location.";
    return true;
  }
  return false;
}

Node * Evaluator::caller(Location loc, unsigned n) {
  Evaluator * frame = this;
  for (; frame != NULL && n != 0; frame = frame->_caller) {
    --n;
  }
  if (frame == NULL || frame->_self == NULL) {
    diag::error(loc) << "Invalid call frame index";
    return &Node::UNDEFINED_NODE;
  }
  return frame->_self;
}

}
