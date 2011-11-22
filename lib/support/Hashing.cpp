/* ================================================================== *
 * Diagnostic functions and macros
 * ================================================================== */

#include "mint/support/Hashing.h"

namespace mint {

/// Hash all of the bytes in the range [first, last)
unsigned hash(const char * first, const char * last) {
  unsigned hash = 2166136261;
  while (first < last) {
    hash = (hash ^ *first++) * 16777619;
  }
  return hash;
}

}
