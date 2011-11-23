/* ================================================================== *
 * The expression evaluator.
 * ================================================================== */

#ifndef MINT_EVAL_EVALUATOR_H
#define MINT_EVAL_EVALUATOR_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
#endif

namespace mint {

class Pool;
class Oper;
class Module;
class Object;
class String;
class TypeRegistry;

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

  // Specific eval functions that take an arbitrary number of arguments.

  Type * evalTypeExpression(Node * ty);
  Node * evalList(Oper * op);
  Node * evalDict(Oper * op);
  Node * evalCall(Oper * op);
  Node * makeObject(Oper * op, String * name);

  /// Evaluate an array of values.
  void evalArgs(NodeArray::iterator src, Node ** dst, size_t count);

  /// Set a property on an object.
  bool setObjectProperty(Object * obj, String * propName, Node * propValue);

  /// Coerce the argument 'n' to type 'ty'.
  Node * coerce(Node * n, Type * ty);

  /// The current scope for resolving variable lookups.
  Node * activeScope() const { return _activeScope; }
  Node * setActiveScope(Node * scope) {
    Node * prevScope = _activeScope;
    _activeScope = scope;
    return prevScope;
  }

  /// Get a pointer to the type registry
  TypeRegistry & typeRegistry() const;

private:
  Module * _module;
  Node * _activeScope;
};

}

#endif // MINT_EVAL_EVALUATOR_H
