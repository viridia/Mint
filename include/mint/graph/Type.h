/* ================================================================== *
 * Type
 * ================================================================== */

#ifndef MINT_GRAPH_TYPE_H
#define MINT_GRAPH_TYPE_H

#ifndef MINT_GRAPH_NODE_H
#include "mint/graph/Node.h"
#endif

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#include "mint/collections/ArrayRef.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

namespace mint {

class Object;

/** -------------------------------------------------------------------------
    A type expression.
 */
class Type : public Node {
public:
  enum TypeKind {
    #define TYPE_KIND(x) x,
    #include "TypeKind.def"
    #undef TYPE_KIND
  };

  /// Constructor
  Type(TypeKind typeKind) : Node(NK_TYPENAME), _typeKind(typeKind) {}
  Type(NodeKind nodeKind, TypeKind typeKind, Location loc)
    : Node(nodeKind, loc, NULL)
    , _typeKind(typeKind)
  {}

  /// What kind of type this is
  TypeKind typeKind() const { return _typeKind; }

  /// Type predicates
  bool isVoidType() const { return _typeKind == Type::VOID; }
  bool isAnyType() const { return _typeKind == Type::ANY; }
  bool isBoolType() const { return _typeKind == Type::BOOL; }
  bool isIntegerType() const { return _typeKind == Type::INTEGER; }
  bool isFloatType() const { return _typeKind == Type::FLOAT; }
  bool isNumberType() const { return _typeKind == Type::INTEGER || _typeKind == Type::FLOAT; }
  bool isStringType() const { return _typeKind == Type::STRING; }
  bool isListType() const { return _typeKind == Type::LIST; }

  // Overrides

  void print(OStream & strm) const;

  static const char * kindName(TypeKind nt);

private:
  TypeKind _typeKind;
};

typedef ArrayRef<Type *> TypeArray;

/** -------------------------------------------------------------------------
    A derived type, such as a List[T].
 */
class DerivedType : public Type {
public:
  typedef TypeArray::const_iterator iterator;
  typedef TypeArray::const_iterator const_iterator;

  /// Creator function.
  static DerivedType * create(TypeKind typeKind, TypeArray params);

  // Type parameters

  TypeArray params() const { return TypeArray(_data, _size); }
  Type * param(unsigned index) const;

  /// Iterators
  const_iterator begin() const { return &_data[0]; }
  const_iterator end() const { return &_data[_size]; }

  size_t size() const { return _size; }

  const Type * const operator[](unsigned index) const { return this->param(index); }

  /// Compute a hash for this derived type.
  unsigned hash() const;

  bool operator==(const DerivedType & dt) const {
    return typeKind() == dt.typeKind() && params() == dt.params();
  }

  bool operator!=(const DerivedType & dt) const {
    return !(*this == dt);
  }

  // Overrides

  void print(OStream & strm) const;
  void trace() const;

private:

  /// Constructor
  DerivedType(TypeKind typeKind, TypeArray params);

  size_t _size;
  Type * _data[1];
};

}

#endif // MINT_GRAPH_TYPE_H
