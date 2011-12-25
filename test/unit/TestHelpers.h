/* ================================================================== *
 * Unit test helpers
 * ================================================================== */

#include "mint/collections/StringRef.h"
#include "mint/collections/SmallString.h"

#include <ostream>

namespace mint {

inline ::std::ostream& operator<<(::std::ostream& os, const StringRef & str) {
  os.write(str.data(), str.size());
  return os;
}

inline void PrintTo(StringRef str, ::std::ostream* os) {
  os->write(str.data(), str.size());
}

}

