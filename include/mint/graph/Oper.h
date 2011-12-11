/* ================================================================== *
 * Operations with arguments
 * ================================================================== */

#ifndef MINT_GRAPH_OPER_H
#define MINT_GRAPH_OPER_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Represents an operation that takes one or more arguments.
 */
class Oper : public Node {
public:
  typedef NodeArray::const_iterator iterator;
  typedef NodeArray::const_iterator const_iterator;

  /// Static creator function for Oper.
  static Oper * create(NodeKind nk, Location location, Type * type, NodeArray args);

  /// Static creator function for Oper.
  static Oper * create(NodeKind nk, Type * type, NodeArray args);

  /// Number of arguments to the operation.
  unsigned size() const { return _size; }

  /// Arguments to this operation
  NodeArray args() const { return NodeArray(_data, _size); }
  Node * arg(unsigned index) const;
  Oper * setArg(unsigned index, Node * value);

  /// Iterators
  const_iterator begin() const { return &_data[0]; }
  const_iterator end() const { return &_data[_size]; }

  // Overrides

  Oper * asOper() { return this; }
  Node * getElement(Node * index) const;
  void print(OStream & strm) const;
  void dump() const;
  void trace() const;

private:
  /// Construct an operator node with the given arguments.
  Oper(NodeKind nk, Location loc, Type * type, NodeArray args);

  size_t _size;
  Node * _data[1];
};

}

#endif // MINT_GRAPH_OPER_H
