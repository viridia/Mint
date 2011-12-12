/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/Directory.h"

#include "mint/support/Diagnostics.h"

namespace mint {

bool Directory::updateDirectoryStatus() {
  _statusValid = path::fileStatus(name()->value(), _status);
  _statusChecked = true;
  if (_status.exists && !_status.isDir) {
    diag::error() << "'" << name()->value() << "' is not a directory.";
  }
  return _statusValid;
}

bool Directory::create() {
  return path::makeDirectoryPath(_name->value());
}

void Directory::trace() const {
  safeMark(_parent);
  safeMark(_name);
  _files.trace();
  _subdirs.trace();
}

}
