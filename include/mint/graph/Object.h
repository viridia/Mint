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
    CACHED = (1<<0),
    PARAM = (1<<1),
  };

  AttributeDefinition(Node * value, Type * type, unsigned flags = PARAM)
    : Node(Node::NK_ATTRDEF, Location(), type)
    , _value(value)
    , _flags(flags)
  {}

  /// The value of this attribute.
  Node * value() const { return _value; }
  void setValue(Node * value) { _value = value; }

  /// True if this attribute should be cached with the configuration
  bool isCached() const { return (_flags & CACHED) != 0; }

  /// True if this attribute is an input parameter
  bool isParam() const { return (_flags & PARAM) != 0; }

  void print(OStream & strm) const;
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

  /// Constructor for type prototypes
  Object(NodeKind nk, TypeKind tk)
    : Type(nk, tk, Location())
    , _definition(NULL)
    , _name(NULL)
    , _parentScope(NULL)
  {
  }

  /// The object that is this object's prototype.
  Object * prototype() const;

  /// Return true if this object has the given prototype somewhere in
  /// it's ancestor chain.
  bool inheritsFrom(Object * proto) const;

  /// The name of this object (can be NULL)
  String * name() const { return _name; }
  void setName(String * name) { _name = name; }
  void setName(StringRef name);

  /// Name of this object or the string 'object'.
  StringRef nameSafe() const { return _name ? _name->value() : "unnamed object"; }

  /// The scope that encloses this object.
  Node * parentScope() const { return _parentScope; }
  void setParentScope(Node * parentScope) { _parentScope = parentScope; }

  /// The table of the object's properties
  const Attributes & attrs() const { return _attrs; }
  Attributes & attrs() { return _attrs; }

  /// Simple function to set the value of an attribute
  void setAttribute(String * attrName, Node * attrValue) {
    _attrs[attrName] = attrValue;
  }

  /// The parse tree for this object - unevaluated
  Node * definition() const { return _definition; }
  void setDefinition(Node * definition) { _definition = definition; }

  /// Define a new attribute on an object. It's an error if an attribute with the
  /// specified name already exists on this object or an ancestor.
  AttributeDefinition * defineAttribute(StringRef name, Node * value = NULL, Type * type = NULL,
      int flags = AttributeDefinition::PARAM);

  /// Define a dynamically-evaluated attribute on an object. It's an error if an attribute
  /// with the specified name already exists on this object or an ancestor.
  AttributeDefinition * defineDynamicAttribute(StringRef name, Type * type, MethodHandler * mh,
      int flags = AttributeDefinition::PARAM);

  /// Define a method on this object.
  Function * defineMethod(StringRef name, StringRef signature, MethodHandler * m);
  Function * defineMethod(StringRef name, Type * returnType, MethodHandler * m);
  Function * defineMethod(StringRef name, Type * returnType, Type * a0, MethodHandler * m);
  Function * defineMethod(StringRef name, Type * returnType, Type * a0, Type * a1,
      MethodHandler * m);
  Function * defineMethod(StringRef name, Type * returnType, Type * a0, Type * a1, Type * a2,
      MethodHandler * m);
  Function * defineMethod(StringRef name, Type * returnType, TypeArray args, MethodHandler * m);

  /// Make a dictionary or scope object
  static Object * makeDict(Object * prototype = NULL, StringRef name = StringRef());

  // Overrides

  Node * getAttributeValue(StringRef name) const;
  bool getAttribute(StringRef name, AttributeLookup & result) const;
  Node * getElement(Node * index) const;
  Object * asObject() { return this; }
  void print(OStream & strm) const;
  void dump() const;
  void trace() const;

protected:
  Node * _definition; // The definition of this object, unevaluated.
  String * _name;
  Node * _parentScope;
  Attributes _attrs;
};

/** -------------------------------------------------------------------------
    TableKeyTraits for Objects.
 */
struct ObjectPointerKeyTraits {
  static inline unsigned hash(const Object * key) {
    return (unsigned(intptr_t(key)) >> 4) ^ (unsigned(intptr_t(key)) >> 9);
  }

  static inline unsigned equals(const Object * l, const Object * r) {
    return l == r;
  }
};

}

#endif // MINT_GRAPH_OBJECT_H
