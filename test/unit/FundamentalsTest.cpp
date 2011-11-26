/* ================================================================== *
 * Fundamentals unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/intrinsic/Fundamentals.h"

namespace mint {

class FundamentalsTest : public testing::Test {
public:
  Fundamentals * fundamentals;

  FundamentalsTest() {
    fundamentals = new Fundamentals();
  }
};

TEST_F(FundamentalsTest, EmptyString) {
  Node * object = fundamentals->properties()[String::create("object")];
  ASSERT_TRUE(object != NULL);
  ASSERT_EQ(Node::NK_OBJECT, object->nodeKind());

  Node * target = fundamentals->properties()[String::create("target")];
  ASSERT_TRUE(target != NULL);
  ASSERT_EQ(Node::NK_OBJECT, target->nodeKind());
}

}
