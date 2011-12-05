/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/Directory.h"
#include "mint/build/Target.h"

namespace mint {

void FSObject::trace() const {
  safeMark(_parent);
  safeMark(_name);
}

}
