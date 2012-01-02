/* ================================================================ *
   Path manipulation functions
 * ================================================================ */

#include "mint/collections/SmallString.h"

#include "mint/support/Assert.h"
#include "mint/support/DirectoryIterator.h"
#include "mint/support/OSError.h"
#include "mint/support/Path.h"

#if HAVE_ERRNO_H
#include <errno.h>
#endif

namespace mint {

#if _WIN32
  DirectoryIterator::DirectoryIterator() : _dirp(INVALID_HANDLE_VALUE) {}

  bool DirectoryIterator::begin(StringRef dirPath) {
    if (_dirp) {
      ::FindClose(_dirp);
    }
    // Create a null-terminated version of the path
    SmallVector<native_char_t, 128> pathBuffer;
    path::toNative(dirPath, pathBuffer);
    using namespace mint::console;

    _dirp = ::FindFirstFileW(pathBuffer.data(), &_findData);
    if (_dirp == INVALID_HANDLE_VALUE) {
      printWin32FileError("accessing", dirPath, ::GetLastError());
      return false;
    }
    return true;
  }

  const char * DirectoryIterator::entryName() const {
    size_t length = ::wcslen(_findData.cFileName);
    path::fromNative(makeArrayRef(_findData.cFileName, length), _entryName);
    return _entryName.cstr();
  }

  bool DirectoryIterator::isDirectory() const {
    return (_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
  }

  bool DirectoryIterator::next() {
    if (_dirp != INVALID_HANDLE_VALUE) {
      if (::FindNextFileW(_dirp, &_findData)) {
        return true;
      } else {
        DWORD result = ::GetLastError();
        if (result != ERROR_NO_MORE_FILES) {
          printWin32FileError("accessing", "directory", result);
        }
      }
    }
    return false;
  }

  void DirectoryIterator::finish() {
    if (_dirp) {
      ::FindClose(_dirp);
    }
    _dirp = INVALID_HANDLE_VALUE;
  }

#elif HAVE_DIRENT_H

  DirectoryIterator::DirectoryIterator() : _dirp(NULL), _entryName(NULL) {}

  bool DirectoryIterator::begin(StringRef dirPath) {
    if (_dirp) {
      ::closedir(_dirp);
    }
    // Create a null-terminated version of the path
    SmallVector<native_char_t, 128> pathBuffer;
    path::toNative(dirPath, pathBuffer);
    using namespace mint::console;

    _dirp = ::opendir(pathBuffer.data());
    if (_dirp == NULL) {
      printPosixFileError("accessing", dirPath, errno);
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
#else
  #error Unimplemented: DirectoryIterator
#endif

}
