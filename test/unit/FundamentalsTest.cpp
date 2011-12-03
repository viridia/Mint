/* ================================================================== *
 * Fundamentals unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"

namespace mint {

using namespace mint::strings;

TEST(FundamentalsTest, BasicProperties) {
  Node * object = Fundamentals::get().attrs()[str("object")];
  ASSERT_TRUE(object != NULL);
  ASSERT_EQ(Node::NK_OBJECT, object->nodeKind());

  Node * target = Fundamentals::get().attrs()[str("target")];
  ASSERT_TRUE(target != NULL);
  ASSERT_EQ(Node::NK_OBJECT, target->nodeKind());
}

}
