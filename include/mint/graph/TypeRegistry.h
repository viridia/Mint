/* ================================================================== *
 * Type
 * ================================================================== */

#ifndef MINT_GRAPH_TYPEREGISTRY_H
#define MINT_GRAPH_TYPEREGISTRY_H

#ifndef MINT_GRAPH_TYPE_H
#include "mint/graph/Type.h"
#endif

#ifndef MINT_COLLECTIONS_TABLE_H
#include "mint/collections/Table.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    TableKeyTraits for derived types. Used to determine if there is already
    a derived type with the given kind and params.
 */
struct DerivedTypeKeyTraits {
  static inline unsigned hash(const DerivedType * dt) {
    return dt->hash();
  }

  static inline unsigned equals(const DerivedType * dtl, const DerivedType * dtr) {
    return dtl == dtr || (dtl->typeKind() == dtr->typeKind() && dtl->params() == dtr->params());
  }
};

/** -------------------------------------------------------------------------
    Registry of all complex types. Acts as a factory to create derived types,
    and storage of all types that have been created.
 */
class TypeRegistry {
public:
  typedef Table<DerivedType, Type, DerivedTypeKeyTraits> DerivedTypeTable;

  /// Constructor
  TypeRegistry() {}

  /// Return the 'any' type.
  static Type * anyType() { return &ANY_TYPE; }

  /// Return the 'boolean' type.
  static Type * boolType() { return &BOOL_TYPE; }

  /// Return the 'integer' type.
  static Type * integerType() { return &INTEGER_TYPE; }

  /// Return the 'float' type.
  static Type * floatType() { return &FLOAT_TYPE; }

  /// Return the 'string' type.
  static Type * stringType() { return &STRING_TYPE; }

  /// Return the 'undefined' type.
  static Type * undefinedType() { return &UNDEFINED_TYPE; }

  /// Return the 'list' generic type.
  static Type * genericListType() { return &GENERIC_LIST_TYPE; }

  /// Return the 'dict' type.
  static Type * genericDictType() { return &GENERIC_DICT_TYPE; }

  /// Create a derived type.
  DerivedType * getDerivedType(Type::TypeKind kind, TypeArray params);

  /// Create a list type.
  DerivedType * getListType(Type * elementType) {
    return getDerivedType(Type::LIST, makeArrayRef(elementType));
  }

  /// Create a dict type.
  DerivedType * getDictType(Type * keyType, Type * valueType) {
    Type * params[] = { keyType, valueType };
    return getDerivedType(Type::DICTIONARY, params);
  }

  /// Create a function type.
  DerivedType * getFunctionType(Type * returnType);
  DerivedType * getFunctionType(Type * returnType, Type * arg0Type);
  DerivedType * getFunctionType(Type * returnType, TypeArray paramTypes);

  /// Garbage collector trace function
  void trace() const;

  // Static type instances

  static Type ANY_TYPE;
  static Type BOOL_TYPE;
  static Type INTEGER_TYPE;
  static Type FLOAT_TYPE;
  static Type STRING_TYPE;
  static Type UNDEFINED_TYPE;
  static Type GENERIC_LIST_TYPE;
  static Type GENERIC_DICT_TYPE;

private:
  // Do not implement. TypeRegistry is non-copyable.
  void operator=(const TypeRegistry &);
  TypeRegistry(const TypeRegistry &);

  /// Table of all derived types.
  DerivedTypeTable _derivedTypes;
};

}

#endif // MINT_GRAPH_TYPEREGISTRY_H
