/* ================================================================== *
 * Memory pool unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/support/Pool.h"

namespace mint {

TEST(PoolTest, EmptyPool) {
  Pool pool;
  EXPECT_EQ(0, pool.size());
  EXPECT_EQ(0, pool.avail());
  EXPECT_EQ(0, pool.unused());
}

TEST(PoolTest, SmallAlloc) {
  Pool pool;
  void * m = pool.alloc(7);
  ASSERT_TRUE(m != NULL);
  EXPECT_EQ(16, pool.size());
  m = pool.alloc(7);
  ASSERT_TRUE(m != NULL);
  EXPECT_EQ(32, pool.size());
}

TEST(PoolTest, Clear) {
  Pool pool;
  void * m = pool.alloc(7);
  pool.clear();
  EXPECT_EQ(0, pool.size());
  m = pool.alloc(7);
  ASSERT_TRUE(m != NULL);
  EXPECT_EQ(16, pool.size());
}

}
