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

Object * TypeSingleton::operator()() {
  if (_ptr == NULL) {
    _ptr = new Object(_nodeKind, _typeKind);
    ((Object *)_ptr)->setName(_name);
  }
  return _ptr;
}

TypeSingleton TypeRegistry::objectType(Node::NK_OBJECT, Type::OBJECT, "object");

TypeSingleton TypeRegistry::optionType(Node::NK_OBJECT, Type::OBJECT, "option");

TypeSingleton TypeRegistry::moduleType(Node::NK_DICT, Type::MODULE, "module");

TypeSingleton TypeRegistry::targetType(Node::NK_OBJECT, Type::OBJECT, "target");

TypeSingleton TypeRegistry::listType(Node::NK_DICT, Type::VOID, "list");

TypeSingleton TypeRegistry::actionType(Node::NK_OBJECT, Type::OBJECT, "action");

TypeSingleton TypeRegistry::dictType(Node::NK_DICT, Type::VOID, "dict");

TypeSingleton TypeRegistry::stringMetaType(Node::NK_DICT, Type::VOID, "string");

Type * TypeRegistry::stringListType() {
  return get().getListType(stringType());
}

DerivedType * TypeRegistry::getDerivedType(Type::TypeKind kind, TypeArray params, Type * meta) {
  for (TypeArray::const_iterator it = params.begin(), itEnd = params.end(); it != itEnd; ++it) {
    M_ASSERT(*it != NULL);
  }
  DerivedType * key = DerivedType::create(kind, params);
  if (meta != NULL) {
    key->setType(meta);
  }
  std::pair<DerivedTypeTable::iterator, bool> it =
      _derivedTypes.insert(std::make_pair(key, (Type *)NULL));
  return it.first->first;
}

DerivedType * TypeRegistry::getListType(Type * elementType) {
  return getDerivedType(Type::LIST, makeArrayRef(elementType), listType());
}

DerivedType * TypeRegistry::getDictType(Type * keyType, Type * valueType) {
  Type * params[] = { keyType, valueType };
  return getDerivedType(Type::DICTIONARY, params, dictType());
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
  static GCPointerRoot<TypeRegistry> instance(new TypeRegistry());
  return *instance;
}

}
