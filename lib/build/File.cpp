/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/File.h"
#include "mint/build/Directory.h"
#include "mint/build/Target.h"

#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"

namespace mint {

bool File::updateFileStatus() {
  _statusValid = path::fileStatus(name()->value(), _status);
  _statusChecked = true;
  if (_status.exists && !_status.isFile) {
    diag::error() << "'" << name()->value() << "' is not a file.";
  }
  return _statusValid;
}

void File::trace() const {
  safeMark(_parent);
  safeMark(_name);
  markArray(ArrayRef<Target *>(_sourceFor));
}

OStream & operator<<(OStream & strm, const File * file) {
  strm << file->name();
  return strm;
}

}
