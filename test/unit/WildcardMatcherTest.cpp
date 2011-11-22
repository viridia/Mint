/* ================================================================== *
 * WildcardMatcher test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/support/Wildcard.h"

namespace mint {

bool doMatch(StringRef pattern, StringRef str) {
  WildcardMatcher matcher(pattern);
  return matcher.match(str);
}

TEST(WildcardMatcherTest, EmptyString) {
  EXPECT_TRUE(doMatch("", ""));
  EXPECT_FALSE(doMatch("a", ""));
  EXPECT_FALSE(doMatch("ab", ""));
}

TEST(WildcardMatcherTest, EmptyPattern) {
  EXPECT_FALSE(doMatch("", "a"));
  EXPECT_FALSE(doMatch("", "aa"));
  EXPECT_FALSE(doMatch("", "ab"));
}

TEST(WildcardMatcherTest, NoWildcards) {
  EXPECT_TRUE(doMatch("ab", "ab"));
  EXPECT_FALSE(doMatch("ab", "aa"));
  EXPECT_FALSE(doMatch("abc", "ab"));
  EXPECT_FALSE(doMatch("ab", "abc"));
}

TEST(WildcardMatcherTest, QMarkWildcard) {
  EXPECT_TRUE(doMatch("?bc", "abc"));
  EXPECT_TRUE(doMatch("a?c", "abc"));
  EXPECT_TRUE(doMatch("ab?", "abc"));
  EXPECT_FALSE(doMatch("?", "abc"));
  EXPECT_FALSE(doMatch("?abc", "abc"));
  EXPECT_FALSE(doMatch("a?bc", "abc"));
  EXPECT_FALSE(doMatch("ab?c", "abc"));
  EXPECT_FALSE(doMatch("abc?", "abc"));
}

TEST(WildcardMatcherTest, StarWildcard) {
  EXPECT_TRUE(doMatch("*abc", "abc"));
  EXPECT_TRUE(doMatch("*bc", "abc"));
  EXPECT_TRUE(doMatch("*c", "abc"));
  EXPECT_TRUE(doMatch("*", "abc"));
  EXPECT_TRUE(doMatch("a*bc", "abc"));
  EXPECT_TRUE(doMatch("a*c", "abc"));
  EXPECT_TRUE(doMatch("a*", "abc"));
  EXPECT_TRUE(doMatch("ab*c", "abc"));
  EXPECT_TRUE(doMatch("ab*", "abc"));
  EXPECT_FALSE(doMatch("*abc", "bc"));
  EXPECT_FALSE(doMatch("*abc", "ab"));
  EXPECT_FALSE(doMatch("*abc", "ac"));
  EXPECT_FALSE(doMatch("*abc", "dbc"));
  EXPECT_FALSE(doMatch("*abc", "dab"));
  EXPECT_FALSE(doMatch("*abc", "dac"));
  EXPECT_FALSE(doMatch("*abc", "abbc"));
  EXPECT_FALSE(doMatch("abc*", "aabc"));
  EXPECT_FALSE(doMatch("abc*", "abbc"));
  EXPECT_FALSE(doMatch("abc*", "ab"));
  EXPECT_FALSE(doMatch("abc*", "abbc"));
}

}
