/* ================================================================== *
 * StringRef unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/collections/StringRef.h"

#include <algorithm>

namespace mint {

TEST(StringRefTest, EmptyString) {
  StringRef sr;
  ASSERT_TRUE(sr.empty());
  ASSERT_EQ(0u, sr.size());
}

TEST(StringRefTest, CString) {
  StringRef sr("xyz");
  ASSERT_FALSE(sr.empty());
  ASSERT_EQ(3u, sr.size());
}

TEST(StringRefTest, Hash) {
  ASSERT_EQ(StringRef("A").hash(), StringRef("A").hash());
  ASSERT_NE(StringRef("A").hash(), StringRef("a").hash());
  ASSERT_NE(StringRef("A").hash(), StringRef("AA").hash());
}

}
