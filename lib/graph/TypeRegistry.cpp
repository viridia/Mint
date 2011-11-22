/* ================================================================== *
 * Node
 * ================================================================== */

#include "mint/graph/TypeRegistry.h"

namespace mint {

Type TypeRegistry::ANY_TYPE(Type::ANY);
Type TypeRegistry::BOOL_TYPE(Type::BOOL);
Type TypeRegistry::INTEGER_TYPE(Type::INTEGER);
Type TypeRegistry::FLOAT_TYPE(Type::FLOAT);
Type TypeRegistry::STRING_TYPE(Type::STRING);
Type TypeRegistry::UNDEFINED_TYPE(Type::ANY);

DerivedType * TypeRegistry::getDerivedType(Type::TypeKind kind, TypeArray params) {
  Ref<DerivedType> key(DerivedType::create(kind, params));
  std::pair<DerivedTypeTable::iterator, bool> it =
      _derivedTypes.insert(std::make_pair(key.ptr(), (Type *)NULL));
  return it.first->first;
}

DerivedType * TypeRegistry::getFunctionType(Type * returnType) {
  SmallVector<Type *, 16> params;
  params.push_back(returnType);
  return getDerivedType(Type::FUNCTION, params);
}

DerivedType * TypeRegistry::getFunctionType(Type * returnType, TypeArray paramTypes) {
  SmallVector<Type *, 16> params;
  params.push_back(returnType);
  params.append(paramTypes.begin(), paramTypes.end());
  return getDerivedType(Type::FUNCTION, params);
}

}
