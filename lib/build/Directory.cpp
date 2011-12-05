/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/Directory.h"

namespace mint {

void Directory::trace() const {
  FSObject::trace();
  _contents.trace();
}

}
