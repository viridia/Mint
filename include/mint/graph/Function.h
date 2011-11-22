/* ================================================================== *
 * Function objects
 * ================================================================== */

#ifndef MINT_GRAPH_FUNCTION_H
#define MINT_GRAPH_FUNCTION_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
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
typedef Node *MethodHandler(Evaluator * ctx, Function * fn, Node * self, NodeArray args);

/** -------------------------------------------------------------------------
    Represents an operation that takes one or more arguments.
 */
class Function : public Node {
public:
  /// Construct a function node with the specified type.
  Function(NodeKind nk, Location loc, Type * type, MethodHandler * handler);

  MethodHandler * handler() const { return _handler; }

  /// The return type of this function.
  Type * returnType() const;

  /// The number of arguments to this function
  unsigned argCount() const;

  /// The type of the Nth argument to this function
  Type * argType(unsigned index) const;

  //virtual Node * eval(Object * object, NodeArray args);

  /// Print a readable version of this node to the stream.
  //void print(OStream & strm) const;

private:
  MethodHandler * _handler;
};

}

#endif // MINT_GRAPH_OPER_H
