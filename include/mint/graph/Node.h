/* ================================================================== *
 * Node
 * ================================================================== */

#ifndef MINT_GRAPH_NODE_H
#define MINT_GRAPH_NODE_H

#ifndef MINT_SUPPORT_GC_H
#include "mint/support/GC.h"
#endif

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#include "mint/collections/ArrayRef.h"
#endif

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

#ifndef MINT_LEX_LOCATION_H
#include "mint/lex/Location.h"
#endif

namespace mint {

class Node;
class Type;
class OStream;
class String;
class AttributeDefinition;

/** -------------------------------------------------------------------------
    Used when looking up the complete information about an attribute.
 */
struct AttributeLookup {
  /// The attribute definition.
  AttributeDefinition * definition;

  /// The current value of the attribute.
  Node * value;

  /// The scope in which the attribute was found.
  Node * foundScope;

  /// Default constructor
  AttributeLookup() : definition(NULL), value(NULL), foundScope(NULL) {}
};

/** -------------------------------------------------------------------------
    Base class of all nodes in the graph
 */
class Node : public GC {
public:
  enum NodeKind {
    #define NODE_KIND(x) NK_##x,
    #define NODE_KIND_RANGE(x, first, last) NK_##x##_FIRST = NK_##first, NK_##x##_LAST = NK_##last,
    #include "NodeKind.def"
    #undef NODE_KIND
    #undef NODE_KIND_RANGE
  };

  /// Constructor
  Node(NodeKind kind) : _nodeKind(kind), _type(NULL) {}
  Node(NodeKind kind, Location location, Type * type)
    : _nodeKind(kind)
    , _location(location)
    , _type(type)
  {}

  /// Destructor
  virtual ~Node() {}

  /// The kind of this node
  NodeKind nodeKind() const { return _nodeKind; }

  /// Where this node was defined
  Location location() const { return _location; }

  /// Type of this value
  Type * type() const { return _type; }
  void setType(Type * ty) { _type = ty; }

  /// Return true if this node is a constant.
  bool isConstant() const {
    return Node::isConstant(_nodeKind);
  }

  /// Return true if this node is the 'undefined' node.
  bool isUndefined() const;

  /// Lookup the value of an attribute on this object. This also searches prototypes.
  virtual Node * getAttributeValue(StringRef name) const;

  /// Lookup the value of an attribute on this object. This also searches prototypes.
  virtual bool getAttribute(StringRef name, AttributeLookup & result) const;

  /// For node types that support array indexing, return the value of the element
  /// keyed by 'index'.
  virtual Node * getElement(Node * index) const;

  /// For nodes that are scopes, this returns the enclosing scope.
  virtual Node * parentScope() const { return NULL; }

  /// Print a readable version of this node to the stream.
  virtual void print(OStream & strm) const;

  /// Print a readable version of this node to stderr.
  virtual void dump() const;

  /// Garbage collection trace function.
  void trace() const;

  /// String name of a node kind.
  static const char * kindName(NodeKind nk);

  /// Return true if this node kind represents a constant.
  static bool isConstant(NodeKind nk);

  /// Return the boolean value 'true' as a Node.
  static Node * boolTrue();

  /// Return the boolean value 'false' as a Node.
  static Node * boolFalse();

  /// Return a boolean value .
  static Node * makeBool(bool value) {
    return value ? boolTrue() : boolFalse();
  }

  /// Make an integer value.
  static Node * makeInt(Location loc, int value);
  static Node * makeInt(int value) { return makeInt(Location(), value); }

  /// Make an float value.
  static Node * makeFloat(Location loc, double value);
  static Node * makeFloat(double value) { return makeFloat(Location(), value); }

  /// Write a text representation of a node to an output stream.
  friend OStream & operator<<(OStream & strm, const Node * n) {
    n->print(strm);
    return strm;
  }

  /// The node that represents the undefined value.
  static Node UNDEFINED_NODE;

private:
  NodeKind _nodeKind;
  Location _location;
  Type * _type;
};

/// Stream operator for NodeKind.
OStream & operator<<(OStream & strm, Node::NodeKind nk);

/// An ArrayRef of nodes.
typedef ArrayRef<Node *> NodeArray;

}

#endif // MINT_GRAPH_NODE_H
