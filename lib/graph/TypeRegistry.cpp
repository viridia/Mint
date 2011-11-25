/* ================================================================== *
 * Node
 * ================================================================== */

#include "mint/graph/TypeRegistry.h"
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

DerivedType * TypeRegistry::getDerivedType(Type::TypeKind kind, TypeArray params) {
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

}
