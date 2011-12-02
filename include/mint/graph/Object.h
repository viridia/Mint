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
struct AttributeDefinition : public Node {
  enum Flags {
    LAZY = (1<<0),
    EXPORT = (1<<1)
  };

  AttributeDefinition(Node * value, Type * type, unsigned flags = 0)
    : Node(Node::NK_PROPDEF, Location(), type)
    , _value(value)
    , _flags(flags)
  {}

  /// The value of this attribute.
  Node * value() const { return _value; }

  /// True if this is a lazily evaluated attribute.
  bool isLazy() const { return (_flags & LAZY) != 0; }

  /// True if this attribute should be exported to the configuration
  bool isExport() const { return (_flags & EXPORT) != 0; }

  void trace() const {
    Node::trace();
    safeMark(_value);
  }
private:
  Node * _value;
  unsigned _flags;
};

typedef StringDict<Node> Attributes;

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
    , _parentScope(NULL)
    , _attrs()
  {
    setType(prototype);
  }

  /// Constructor
  Object(NodeKind nk, Location location, Object * prototype)
    : Type(nk, Type::OBJECT, location)
    , _definition(NULL)
    , _name(NULL)
    , _parentScope(NULL)
  {
    setType(prototype);
  }

  /// The object that is this object's prototype.
  Object * prototype() const;

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
  const Attributes & attrs() const { return _attrs; }
  Attributes & attrs() { return _attrs; }

  /// The parse tree for this object - unevaluated
  Node * definition() const { return _definition; }
  void clearDefinition() { _definition = NULL; }

  /// Lookup the value of an attribute on this object. This also searches prototypes.
  Node * getAttributeValue(StringRef name) const;

  /// Lookup the definition of an attribute on the object. This also searches prototypes.
  AttributeDefinition * getPropertyDefinition(StringRef name) const;

  /// Return true this object has a value for attribute 'name', not including inherited attributes.
  bool hasPropertyImmediate(StringRef name) const;

  /// Define a new attribute on an object. It's an error if an attribute with the
  /// specified name already exists on this object or an ancestor.
  AttributeDefinition * defineAttribute(String * name, Node * value = NULL, Type * type = NULL,
      unsigned lazy = 0);

  /// Define a dynamically-evaluated attribute on an object. It's an error if an attribute
  /// with the specified name already exists on this object or an ancestor.
  AttributeDefinition * defineDynamicAttribute(StringRef name, Type * type, MethodHandler * mh);

  /// Define a method on this object.
  void defineMethod(StringRef name, Type * returnType, MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, Type * a0, MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, Type * a0, Type * a1, MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, Type * a0, Type * a1, Type * a2,
      MethodHandler * m);
  void defineMethod(StringRef name, Type * returnType, TypeArray args, MethodHandler * m);

  /// Make a dictionary or scope object
  static Object * makeDict(Object * prototype = NULL, StringRef name = StringRef());

  // Overrides

  bool getAttribute(StringRef name, AttributeLookup & result) const;
  Node * getElement(Node * index) const;
  void print(OStream & strm) const;
  void dump() const;
  void trace() const;

private:
  friend class ObjectBuilder;

  Node * _definition; // The definition of this object, unevaluated.
  String * _name;
  Node * _parentScope;
  Attributes _attrs;
};

}

#endif // MINT_GRAPH_OBJECT_H
