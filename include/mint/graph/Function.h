/* ================================================================== *
 * Function objects
 * ================================================================== */

#ifndef MINT_GRAPH_FUNCTION_H
#define MINT_GRAPH_FUNCTION_H

#ifndef MINT_GRAPH_STRING_H
#include "mint/graph/String.h"
#endif

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#include "mint/collections/ArrayRef.h"
#endif

namespace mint {

class Object;
class Function;
class Evaluator;

/** -------------------------------------------------------------------------
    C++ method which is called when a function is evaluated.
 */
typedef Node *MethodHandler(
    Location loc, Evaluator * ctx, Function * fn, Node * self, NodeArray args);

/** -------------------------------------------------------------------------
    Represents a function parameter definition.
 */
class Parameter {
public:
  /// Default constructor
  Parameter() : _name(NULL), _variadic(false) {}

  /// Constructor from a parameter name.
  Parameter(String * name) : _name(name), _variadic(false) {}

  /// Copy constructor
  Parameter(const Parameter & src) : _name(src._name), _variadic(src._variadic)  {}

  /// Assignment operator
  const Parameter & operator=(const Parameter & src) {
    _name = src._name;
    _variadic = src._variadic;
    return *this;
  }

  /// The name of the parameter. */
  String * name() const { return _name; }
  Parameter & setName(String * name) {
    _name = name;
    return *this;
  }

  bool isVariadic() const { return _variadic; }
  void setVariadic(bool variadic) { _variadic = variadic; }

  void trace() const {
    _name->mark();
  }

private:
  String * _name;
  bool _variadic;
};

typedef SmallVectorImpl<Parameter> ParameterList;

/** -------------------------------------------------------------------------
    Represents an operation that takes one or more arguments.
 */
class Function : public Node {
public:

  /// Construct a function node with the specified type.
  Function(NodeKind nk, Location loc, Type * type, MethodHandler * handler);

  /// Construct a function node with the specified type.
  Function(NodeKind nk, Location loc, Type * type, const SmallVectorImpl<Parameter> & params,
      MethodHandler * handler);

  /// Name of this function
  String * name() const { return _name; }
  void setName(String * name) { _name = name; }

  /// Native handler code for this function.
  MethodHandler * handler() const { return _handler; }

  /// The return type of this function.
  Type * returnType() const;

  /// The number of arguments to this function
  unsigned argCount() const;

  /// The type of the Nth argument to this function
  Type * argType(unsigned index) const;

  /// Scope in which this function was defined.
  Node * parentScope() const { return _parentScope; }
  void setParentScope(Node * parentScope) { _parentScope = parentScope; }

  /// Function parameter definitions
  const ParameterList & params() const { return _params; }
  ParameterList & params() { return _params; }

  /// Nodes that make up the body of the function.
  Node * body() const { return _body; }
  void setBody(Node * body) { _body = body; }

  void trace() const;

  /// Print a readable version of this node to the stream.
  void print(OStream & strm) const;

private:
  String * _name;
  MethodHandler * _handler;
  Node * _parentScope;
  SmallVector<Parameter, 4> _params;
  Node * _body;
};

}

#endif // MINT_GRAPH_OPER_H
