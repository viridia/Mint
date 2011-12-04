/* ================================================================== *
 * Fundamentals unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

namespace mint {

using namespace mint::strings;

TEST(FundamentalsTest, BasicProperties) {
  Node * object = Fundamentals::get().attrs()[str("object")];
  ASSERT_TRUE(object != NULL);
  ASSERT_EQ(Node::NK_OBJECT, object->nodeKind());

  Node * target = Fundamentals::get().attrs()[str("target")];
  ASSERT_TRUE(target != NULL);
  ASSERT_EQ(Node::NK_OBJECT, target->nodeKind());

  Node * option = TypeRegistry::optionType();
  ASSERT_TRUE(option != NULL);
  ASSERT_EQ(Node::NK_DICT, option->nodeKind());
  ASSERT_TRUE(option->getAttributeValue("help") != NULL);

  Node * listType = TypeRegistry::listType();
  ASSERT_TRUE(listType != NULL);
  ASSERT_EQ(Node::NK_DICT, listType->nodeKind());
  ASSERT_TRUE(listType->getAttributeValue("map") != NULL);
}

}
