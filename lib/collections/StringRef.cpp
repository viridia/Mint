/* ================================================================== *
 * StringRef
 * ================================================================== */

#include "mint/collections/StringRef.h"

namespace mint {

// Simple FNV-1 hash
unsigned StringRef::hash() const {
  const_iterator s = begin();
  const_iterator e = end();

  unsigned hash = 2166136261;
  while (s < e) {
    hash = (hash ^ *s++) * 16777619;
  }
  return hash;
}

}
