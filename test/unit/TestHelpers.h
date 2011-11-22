/* ================================================================== *
 * Unit test helpers
 * ================================================================== */

#include "mint/collections/StringRef.h"

#include <ostream>

namespace mint {

inline ::std::ostream& operator<<(::std::ostream& os, const StringRef & str) {
  os.write(str.data(), str.size());
}

}

