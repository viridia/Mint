/* ================================================================== *
 * Main unit test module.
 * ================================================================== */

#include "gtest/gtest.h"

int main(int argc, char **argv) {
  // Initialize and run tests.
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
