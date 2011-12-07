/* ================================================================== *
 * The expression evaluator.
 * ================================================================== */

#ifndef MINT_EVAL_EVALUATOR_H
#define MINT_EVAL_EVALUATOR_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
#endif

namespace mint {

class Function;
class Module;
class Object;
class Oper;
class String;
class TypeRegistry;
class Fundamentals;

/** -------------------------------------------------------------------------
    The expression evaluator.
 */
class Evaluator {
public:
  /// Constructor.
  /// Parameters:
  ///   module: The current module. Expressions are evaluated in the context
  ///           of this module.
  Evaluator(Module * module);
  Evaluator(Evaluator & parent);

  /// The current module
  Module * module() const { return _module; }

  /// Evaluate node 'n' and return the result.
  Node * eval(Node * n);

  /// Evaluate the arguments of 'content' in the context of module 'mod'.
  bool evalModuleContents(Oper * content);
  bool evalModuleAttribute(Oper * op);
  bool evalOption(Node * parent, Oper * op);

  /// Fill in the body of an object
  bool evalObjectContents(Object * obj);
  Node * evalAttribute(
      Location loc, AttributeLookup & attrLookup, Node * searchScope, StringRef name);

  /// Return an evaluated attribute value. Returns NULL if there is no such attribute.
  Node * attributeValue(Node * searchScope, StringRef name);

  /// Call a function
  Node * call(Location loc, Node * callable, Node * self, NodeArray args);

  // Specific eval functions that take an arbitrary number of arguments.

  Type * evalTypeExpression(Node * ty);
  Node * evalList(Oper * op);
  Node * evalDict(Oper * op);
  Node * evalCall(Oper * op);
  Node * evalConcat(Oper * op);
  Node * evalDoStmt(Oper * op);
  Node * evalLetStmt(Oper * op);
  Node * makeObject(Oper * op, String * name);

  /// Evaluate an array of values.
  void evalArgs(NodeArray::iterator src, Node ** dst, size_t count);

  /// Return true if two nodes have equal value
  bool equal(Location loc, Node * lhs, Node * rhs);

  /// Return 0, 1, or -1 for comparing the content of two nodes.
  int compare(Location loc, Node * lhs, Node * rhs);

  /// Set an attribute on an object.
  bool setAttribute(Object * obj, String * attrName, Node * attrValue);

  /// Return true if the input type is not one of the values convertible to 'false'.
  bool isNonNil(Node * n);

  /// Coerce the argument 'n' to type 'ty'.
  Node * coerce(Node * n, Type * ty);
  bool coerceArgs(Location loc, Node * callable, SmallVectorImpl<Node *> & args);

  /// Given two types, select a common type which encompasses both.
  Type * selectCommonType(Type * t0, Type * t1);

  /// Given a path, import a module at that path.
  Module * importModule(Node * path);
  Node * importSymbol(Node * path);

  /// The current scope for resolving variable lookups.
  Node * lexicalScope() const { return _lexicalScope; }
  Node * setLexicalScope(Node * scope) {
    Node * prevScope = _lexicalScope;
    _lexicalScope = scope;
    return prevScope;
  }

  /// The pointer to the current 'self' object.
  Node * self() const { return _self; }
  Node * setSelf(Node * scope) {
    Node * prevScope = _self;
    _self= scope;
    return prevScope;
  }

  /// Get a pointer to the type registry
  TypeRegistry & typeRegistry() const { return _typeRegistry; }

  /// Return true if 'name' is already defined in 'scope'.
  bool checkAlreadyDefined(Location loc, Node * scope, StringRef name);

  /// Return the 'self' value from the nth call frame.
  Node * caller(Location loc, unsigned n);

private:
  Node * lookupIdent(StringRef name, AttributeLookup & lookup);
  Node * createDeferred(Oper * deferred, Type * type);

  /// Callback function to evaluate the body of a function.
  static Node * evalFunctionBody(Location loc, Evaluator * ex, Function * fn, Node * self,
      NodeArray args);

  Module * _module;
  TypeRegistry & _typeRegistry;
  Node * _lexicalScope;
  Node * _self;
  Evaluator * _caller;
};

}

#endif // MINT_EVAL_EVALUATOR_H
