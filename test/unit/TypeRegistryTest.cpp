/* ================================================================== *
 * TypeRegistry unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/graph/TypeRegistry.h"

namespace mint {

class TypeRegistryTest : public testing::Test {
public:
  TypeRegistry reg;

  TypeRegistryTest() {}
};

TEST_F(TypeRegistryTest, ListType) {
  DerivedType * listType = reg.getListType(reg.boolType());
  ASSERT_EQ(Type::LIST, listType->typeKind());
  ASSERT_EQ(1u, listType->size());
  ASSERT_EQ(Type::BOOL, (*listType)[0]->typeKind());

  DerivedType * listType2 = reg.getListType(reg.boolType());
  ASSERT_TRUE(listType == listType2);

  DerivedType * listType3 = reg.getListType(reg.integerType());
  ASSERT_TRUE(listType != listType3);
}

TEST_F(TypeRegistryTest, FunctionType) {
  DerivedType * funcType = reg.getFunctionType(reg.integerType(), makeArrayRef(reg.floatType()));
  ASSERT_EQ(Type::FUNCTION, funcType->typeKind());
  ASSERT_EQ(2u, funcType->size());
  ASSERT_EQ(Type::INTEGER, (*funcType)[0]->typeKind());
  ASSERT_EQ(Type::FLOAT, (*funcType)[1]->typeKind());

  DerivedType * funcType2 = reg.getFunctionType(reg.integerType(), makeArrayRef(reg.floatType()));
  ASSERT_TRUE(funcType == funcType2);

  DerivedType * funcType3 = reg.getFunctionType(reg.integerType(), makeArrayRef(reg.stringType()));
  ASSERT_TRUE(funcType != funcType3);
}

}
