/* ================================================================== *
 * OStream unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/support/OStream.h"

#include <algorithm>

namespace mint {

TEST(StringRefTest, OStrStrem) {
  OStrStream strm;
  strm << "abc";
  EXPECT_EQ(3u, strm.str().size());
}

}
