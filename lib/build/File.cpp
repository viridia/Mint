/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/File.h"
#include "mint/build/Target.h"

#include "mint/support/Path.h"

namespace mint {

bool File::updateFileStatus() {
  _statusValid = path::fileStatus(name()->value(), _status);
  _statusChecked = true;
  return _statusValid;
}

void File::trace() const {
  FSObject::trace();
  markArray(ArrayRef<Target *>(_sourceFor));
}

OStream & operator<<(OStream & strm, const File * file) {
  strm << file->name();
  return strm;
}

}
