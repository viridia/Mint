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
  Evaluator(Module * module, TypeRegistry & typeRegistry);

  /// The current module
  Module * module() const { return _module; }

  /// Evaluate node 'n' and return the result.
  Node * eval(Node * n);

  /// Evaluate the arguments of 'content' in the context of module 'mod'.
  bool evalModuleContents(Oper * content);
  bool evalModuleProperty(Oper * op);
  bool evalModuleOption(Oper * op);

  bool checkModulePropertyDefined(String * propName);

  /// Fill in the body of an object
  bool evalObjectContents(Object * obj);
  Node * evalObjectProperty(Location loc, Node * obj, StringRef name);

  // If an object has an inherited property, evaluate it (if lazy) and set the
  // result on the object itself.
//  Node * realizeObjectProperty(Location loc, Object * obj, StringRef name);

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

  /// Set a property on an object.
  bool setObjectProperty(Object * obj, String * propName, Node * propValue);

  /// Return true if the input type is not one of the values convertible to 'false'.
  bool isNonNil(Node * n);

  /// Coerce the argument 'n' to type 'ty'.
  Node * coerce(Node * n, Type * ty);
  bool coerceArgs(Function * fn, SmallVectorImpl<Node *> & args);

  /// Given two types, select a common type which encompasses both.
  Type * selectCommonType(Type * t0, Type * t1);

  /// Given a path, import a module at that path.
  Module * importModule(Node * path);

  /// The current scope for resolving variable lookups.
  Node * activeScope() const { return _activeScope; }
  Node * setActiveScope(Node * scope) {
    Node * prevScope = _activeScope;
    _activeScope = scope;
    return prevScope;
  }

  /// Get a pointer to the type registry
  TypeRegistry & typeRegistry() const { return _typeRegistry; }

private:

  /// Callback function to evaluate the body of a function.
  static Node * evalFunctionBody(Evaluator * ex, Function * fn, Node * self, NodeArray args);

  Module * _module;
  TypeRegistry & _typeRegistry;
  Node * _activeScope;
};

}

#endif // MINT_EVAL_EVALUATOR_H
