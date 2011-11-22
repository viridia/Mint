/* ================================================================== *
 * Literals - int, float, string
 * ================================================================== */

#ifndef MINT_GRAPH_LITERAL_H
#define MINT_GRAPH_LITERAL_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    A literal value.
 */
template<class T>
class Literal : public Node {
public:

  /// Construct a literal value.
  Literal(NodeKind nt, Location location, Type * ty, T value)
    : Node(nt, location, ty)
    , _value(value)
  {}

  /// The value of this literal
  T value() const { return _value; }

private:
  T _value;
};

}

#endif // MINT_GRAPH_LITERAL_H
