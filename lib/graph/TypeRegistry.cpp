/* ================================================================== *
 * Node
 * ================================================================== */

#include "mint/graph/Object.h"

#include "mint/intrinsic/TypeRegistry.h"

#include "mint/support/Assert.h"

namespace mint {

Type TypeRegistry::ANY_TYPE(Type::ANY);
Type TypeRegistry::BOOL_TYPE(Type::BOOL);
Type TypeRegistry::INTEGER_TYPE(Type::INTEGER);
Type TypeRegistry::FLOAT_TYPE(Type::FLOAT);
Type TypeRegistry::STRING_TYPE(Type::STRING);
Type TypeRegistry::UNDEFINED_TYPE(Type::ANY);
Type TypeRegistry::GENERIC_LIST_TYPE(Type::VOID);
Type TypeRegistry::GENERIC_DICT_TYPE(Type::VOID);

Object * TypeRegistry::moduleType() {
  static GCPointerRoot<Object> type = new Object(Node::NK_DICT, Location(), NULL);
  return type;
}

Object * TypeRegistry::listType() {
  static GCPointerRoot<Object> type = new Object(Node::NK_DICT, Location(), NULL);
  return type;
}

Object * TypeRegistry::dictType() {
  static GCPointerRoot<Object> type = new Object(Node::NK_DICT, Location(), NULL);
  return type;
}

DerivedType * TypeRegistry::getDerivedType(Type::TypeKind kind, TypeArray params) {
  for (TypeArray::const_iterator it = params.begin(), itEnd = params.end(); it != itEnd; ++it) {
    M_ASSERT(*it != NULL);
  }
  DerivedType * key = DerivedType::create(kind, params);
  std::pair<DerivedTypeTable::iterator, bool> it =
      _derivedTypes.insert(std::make_pair(key, (Type *)NULL));
  return it.first->first;
}

DerivedType * TypeRegistry::getFunctionType(Type * returnType) {
  SmallVector<Type *, 16> params;
  params.push_back(returnType);
  return getDerivedType(Type::FUNCTION, params);
}

DerivedType * TypeRegistry::getFunctionType(Type * returnType, Type * arg0Type) {
  M_ASSERT(returnType != NULL);
  Type * args[] = { arg0Type };
  return getFunctionType(returnType, args);
}

DerivedType * TypeRegistry::getFunctionType(Type * returnType, TypeArray paramTypes) {
  M_ASSERT(returnType != NULL);
  SmallVector<Type *, 16> params;
  params.push_back(returnType);
  params.append(paramTypes.begin(), paramTypes.end());
  return getDerivedType(Type::FUNCTION, params);
}

void TypeRegistry::trace() const {
  for (DerivedTypeTable::const_iterator it = _derivedTypes.begin(), itEnd = _derivedTypes.end();
      it != itEnd; ++it) {
    it->first->mark();
  }
}

TypeRegistry & TypeRegistry::get() {
  static GCPointerRoot<TypeRegistry> instance = new TypeRegistry();
  return *instance;
}

}
