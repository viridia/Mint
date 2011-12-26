/* ================================================================== *
 * Type
 * ================================================================== */

#ifndef MINT_INTRINSIC_TYPEREGISTRY_H
#define MINT_INTRINSIC_TYPEREGISTRY_H

#ifndef MINT_GRAPH_TYPE_H
#include "mint/graph/Type.h"
#endif

#ifndef MINT_GRAPH_OBJECT_H
#include "mint/graph/Object.h"
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
    Class that is used to declare singleton objects that are the base for
    object types.
 */
class TypeSingleton {
public:
  TypeSingleton(Node::NodeKind nodeKind, Type::TypeKind typeKind, const char * name)
    : _nodeKind(nodeKind)
    , _typeKind(typeKind)
    , _name(name)
    , _ptr(NULL)
  {}

  /// The call operator lazily creates and returns the object.
  Object * operator()();
private:
  Node::NodeKind _nodeKind;
  Type::TypeKind _typeKind;
  const char * _name;
  GCPointerRoot<Object> _ptr;
};

/** -------------------------------------------------------------------------
    Registry of all complex types. Acts as a factory to create derived types,
    and storage of all types that have been created.
 */
class TypeRegistry : public GC {
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

  /// The base type of objects.
  static TypeSingleton objectType;

  /// The base type of options.
  static TypeSingleton optionType;

  /// The base type of modules.
  static TypeSingleton moduleType;

  /// The base type of targets.
  static TypeSingleton targetType;

  /// The base type of actions.
  static TypeSingleton actionType;

  /// The base type of all lists.
  static TypeSingleton listType;

  /// The base type of all dicts.
  static TypeSingleton dictType;

  /// Used to hold methods for string.
  static TypeSingleton stringMetaType;

  /// Return the 'list' generic type.
  static Type * genericListType() { return &GENERIC_LIST_TYPE; }

  /// Return the 'dict' type.
  static Type * genericDictType() { return &GENERIC_DICT_TYPE; }

  /// A list of strings.
  static Type * stringListType();

  /// Create a derived type.
  DerivedType * getDerivedType(Type::TypeKind kind, TypeArray params, Type * meta = NULL);

  /// Create a list type.
  DerivedType * getListType(Type * elementType);

  /// Create a dict type.
  DerivedType * getDictType(Type * keyType, Type * valueType);

  /// Create a function type.
  DerivedType * getFunctionType(Type * returnType);
  DerivedType * getFunctionType(Type * returnType, Type * arg0Type);
  DerivedType * getFunctionType(Type * returnType, TypeArray paramTypes);

  /// Parse a encoded type expression and return the resulting type, along with the updated
  /// iterator.
  Type * parseTypeCode(StringRef typeCode, size_t & pos);

  /// Garbage collector trace function
  void trace() const;

  /// Return the TypeRegistry singleton.
  static TypeRegistry & get();

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

#endif // MINT_INTRINSIC_TYPEREGISTRY_H
