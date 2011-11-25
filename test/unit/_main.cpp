/* ================================================================== *
 * Main unit test module.
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/support/GC.h"

int main(int argc, char **argv) {
  mint::GC::init();
  // Initialize and run tests.
  testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  mint::GC::uninit();
  return result;
}
