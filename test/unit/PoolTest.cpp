/* ================================================================== *
 * Memory pool unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/support/Pool.h"

namespace mint {

TEST(PoolTest, EmptyPool) {
  Pool pool;
  EXPECT_EQ(0u, pool.size());
  EXPECT_EQ(0u, pool.avail());
  EXPECT_EQ(0u, pool.unused());
}

TEST(PoolTest, SmallAlloc) {
  Pool pool;
  void * m = pool.alloc(7);
  ASSERT_TRUE(m != NULL);
  EXPECT_EQ(16u, pool.size());
  m = pool.alloc(7);
  ASSERT_TRUE(m != NULL);
  EXPECT_EQ(32u, pool.size());
}

TEST(PoolTest, Clear) {
  Pool pool;
  void * m = pool.alloc(7);
  pool.clear();
  EXPECT_EQ(0u, pool.size());
  m = pool.alloc(7);
  ASSERT_TRUE(m != NULL);
  EXPECT_EQ(16u, pool.size());
}

}
