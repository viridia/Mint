/* ================================================================== *
 * Object
 * ================================================================== */

#ifndef MINT_GRAPH_OBJECT_H
#define MINT_GRAPH_OBJECT_H

#ifndef MINT_GRAPH_TYPE_H
#include "mint/graph/Type.h"
#endif

#ifndef MINT_GRAPH_FUNCTION_H
#include "mint/graph/Function.h"
#endif

#ifndef MINT_GRAPH_STRINGDICT_H
#include "mint/graph/StringDict.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

namespace mint {

class Object;
class Type;

/** -------------------------------------------------------------------------
    Represents a field definition within an object.
 */
struct Property : public Node {

  Property(Node * value, Type * type, bool lazy = false)
    : Node(Node::NK_PROPDEF, Location(), type)
    , _value(value)
    , _lazy(lazy)
  {}

  /// The value of this property.
  Node * value() const { return _value; }

  /// True if this is a lazily evaluated property.
  bool lazy() const { return _lazy; }

private:
  Node * _value;
  bool _lazy;
};

typedef StringDict<Node> PropertyTable;

/** -------------------------------------------------------------------------
    An object instance. Note that all objects are also types.
 */
class Object : public Type {
public:

  /// Constructor
  Object(Location location, Object * prototype)
    : Type(Node::NK_OBJECT, Type::OBJECT, location)
    , _parentScope(NULL)
    , _prototype(prototype)
    , _properties()
  {}

  /// Constructor
  Object(NodeKind nk, Location location, Object * prototype)
    : Type(nk, Type::OBJECT, location)
    , _parentScope(NULL)
    , _prototype(prototype)
    , _properties()
  {}

  /// The object that is this object's prototype.
  Object * prototype() const { return _prototype.ptr(); }

  /// The name of this object (can be NULL)
  String * name() const { return _name.ptr(); }
  void setName(String * name) { _name = name; }

  /// Name of this object or the string 'object'.
  StringRef nameSafe() const { return _name.ptr() ? _name->value() : "unnamed object"; }

  /// The scope that encloses this object.
  Node * parentScope() const { return _parentScope; }
  void setParentScope(Node * parentScope) { _parentScope = parentScope; }

  /// The table of the object's properties
  const PropertyTable & properties() const { return _properties; }
  PropertyTable & properties() { return _properties; }

  /// Lookup the value of a property on this object. This also searches prototypes.
  Node * getPropertyValue(String * name) const;
  Node * getPropertyValue(StringRef name) const;

  /// Define a new property on an object. It's an error if a property with the
  /// specified name already exists on this object or an ancestor.
  Property * defineProperty(String * name, Node * value = NULL, Type * type = NULL,
      bool lazy = false);

  /// Lookup an object property (with prototype inheritance.)
  Property * findProperty(String * name) const;

  /// Print a readable version of this node to the stream.
  void print(OStream & strm) const;

  /// Print a readable version of this object to stderr.
  void dump() const;

private:
  friend class ObjectBuilder;

  Node * _parentScope; // This should not be a ref because parents point to children.
  Ref<Object> _prototype;
  Ref<String> _name;
  PropertyTable _properties;
};

}

#endif // MINT_GRAPH_OBJECT_H
