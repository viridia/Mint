/* ================================================================ *
   Path manipulation functions
 * ================================================================ */

#include "mint/collections/SmallString.h"

#include "mint/support/Assert.h"
#include "mint/support/Directory.h"
#include "mint/support/OSError.h"

#if HAVE_ERRNO_H
#include <errno.h>
#endif

namespace mint {

#if HAVE_DIRENT_H
  DirectoryIterator::DirectoryIterator() : _dirp(NULL), _entryName(NULL) {}

  bool DirectoryIterator::begin(StringRef dirPath) {
    if (_dirp) {
      ::closedir(_dirp);
    }
    // Create a null-terminated version of the path
    SmallString<128> pathBuffer(dirPath);
    pathBuffer.push_back('\0');
    using namespace mint::console;

    _dirp = ::opendir(pathBuffer.data());
    if (_dirp == NULL) {
      printPosixFileError(dirPath, errno);
      return false;
    }
    return true;
  }

  const char * DirectoryIterator::entryName() const {
    return _entryName;
  }

  bool DirectoryIterator::isDirectory() const {
    #if !DIRENT_HAS_D_TYPE
      M_ASSERT(false) << "Implement isDirectory() for systems with no dirent::d_type field.";
    #endif
    return _isDirectory;
  }

  bool DirectoryIterator::next() {
    if (_dirp) {
      ::dirent * de = ::readdir(_dirp);
      if (de != NULL) {
        _entryName = de->d_name;
        #if DIRENT_HAS_D_TYPE
          _isDirectory = (de->d_type == DT_DIR);
        #endif
        return true;
      } else {
        _entryName = NULL;
      }
    }
    return false;
  }

  void DirectoryIterator::finish() {
    if (_dirp) {
      ::closedir(_dirp);
    }
    _dirp = NULL;
    _entryName = NULL;
  }
#endif

}
