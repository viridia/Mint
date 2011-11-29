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
  enum PropertyFlags {
    LAZY = (1<<0),
    EXPORT = (1<<1)
  };

  Property(Node * value, Type * type, unsigned flags = 0)
    : Node(Node::NK_PROPDEF, Location(), type)
    , _value(value)
    , _flags(flags)
  {}

  /// The value of this property.
  Node * value() const { return _value; }

  /// True if this is a lazily evaluated property.
  bool isLazy() const { return (_flags & LAZY) != 0; }

  /// True if this property should be exported to the configuration
  bool isExport() const { return (_flags & EXPORT) != 0; }

  void trace() const {
    Node::trace();
    safeMark(_value);
  }
private:
  Node * _value;
  unsigned _flags;
};

typedef StringDict<Node> PropertyTable;

/** -------------------------------------------------------------------------
    An object instance. Note that all objects are also types.
 */
class Object : public Type {
public:

  /// Constructor
  Object(Location location, Object * prototype, Node * definition = NULL)
    : Type(Node::NK_OBJECT, Type::OBJECT, location)
    , _definition(definition)
    , _name(NULL)
    , _prototype(prototype)
    , _parentScope(NULL)
    , _properties()
  {}

  /// Constructor
  Object(NodeKind nk, Location location, Object * prototype)
    : Type(nk, Type::OBJECT, location)
    , _definition(NULL)
    , _name(NULL)
    , _prototype(prototype)
    , _parentScope(NULL)
  {}

  /// The object that is this object's prototype.
  Object * prototype() const { return _prototype; }

  /// Return true if this object has the given prototype somewhere in
  /// it's ancestor chain.
  bool inheritsFrom(Object * proto) const;

  /// The name of this object (can be NULL)
  String * name() const { return _name; }
  void setName(String * name) { _name = name; }

  /// Name of this object or the string 'object'.
  StringRef nameSafe() const { return _name ? _name->value() : "unnamed object"; }

  /// The scope that encloses this object.
  Node * parentScope() const { return _parentScope; }
  void setParentScope(Node * parentScope) { _parentScope = parentScope; }

  /// The table of the object's properties
  const PropertyTable & properties() const { return _properties; }
  PropertyTable & properties() { return _properties; }

  /// The parse tree for this object - unevaluated
  Node * definition() const { return _definition; }
  void clearDefinition() { _definition = NULL; }

  /// Lookup the value of a property on this object. This also searches prototypes.
  Node * getPropertyValue(StringRef name) const;

  /// Lookup the definition of a property on the object. This also searches prototypes.
  Property * getPropertyDefinition(StringRef name) const;

  /// Return true this object has a value for property 'name', not including inherited properties.
  bool hasPropertyImmediate(StringRef name) const;

  /// Define a new property on an object. It's an error if a property with the
  /// specified name already exists on this object or an ancestor.
  Property * defineProperty(String * name, Node * value = NULL, Type * type = NULL,
      unsigned lazy = 0);

  /// Define a method on this object.
  void defineMethod(StringRef name, Type * returnType, MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, Type * a0, MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, Type * a0, Type * a1, MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, Type * a0, Type * a1, Type * a2,
      MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, TypeArray args, MethodHandler * m);

  // Overrides

  void print(OStream & strm) const;
  void dump() const;
  void trace() const;

private:
  friend class ObjectBuilder;

  Node * _definition; // The definition of this object, unevaluated.
  String * _name;
  Object * _prototype;
  Node * _parentScope;
  PropertyTable _properties;
};

}

#endif // MINT_GRAPH_OBJECT_H
