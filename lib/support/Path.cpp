/* ================================================================ *
   Path manipulation functions
 * ================================================================ */

#include "mint/collections/SmallString.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OSError.h"
#include "mint/support/Path.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_SYS_UNISTD_H
#include <sys/unistd.h>
#endif

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_IO_H
#include <io.h>
#endif

#if defined(_WIN32)
  #include <windows.h>
  #undef min
  #define R_OK 04
  #define W_OK 02
  typedef SSIZE_T ssize_t;
#endif

#if defined(_MSC_VER)
  #pragma warning(disable:4996) 
#endif

namespace mint {
namespace path {

#define DIRECTORY_SEPARATOR_CHAR '/'

namespace {
  inline void append(SmallVectorImpl<char> & out, StringRef str) {
    out.append(str.begin(), str.end());
  }

  inline void assign(SmallVectorImpl<char> & out, StringRef str) {
    out.clear();
    out.append(str.begin(), str.end());
  }

  static StringRef DIRSEP("/", 1);
#if !defined(_WIN32)
  int openFileForRead(StringRef path) {
    // Create a null-terminated version of the path
    SmallString<128> pathBuffer(path.begin(), path.end());
    pathBuffer.push_back('\0');
    using namespace mint::console;

    int fd = ::open(pathBuffer.data(), O_RDONLY);
    if (fd == -1) {
      int error = errno;
      if (error == ENOENT) {
        err() << "Error opening '" << path << "' for reading: file not found.\n";
      } else if (error == EACCES) {
        err() << "Error opening '" << path << "' for reading: permission denied.\n";
      } else {
        printPosixFileError("reading", path, error);
      }
    }

    return fd;
  }

  int openFileForWrite(StringRef path) {
    SmallString<128> pathBuffer(path);
    pathBuffer.push_back('\0');
    int fd = ::open(pathBuffer.data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
      int error = errno;
      if (error == EACCES) {
        console::err() << "Error opening '" << path << "' for writing: permission denied.\n";
      } else {
        printPosixFileError("writing", path, error);
      }
      return -1;
    }
    return fd;
  }
#endif
}
int findSeparatorFwd(const char * path, size_t size, int pos) {
  while (pos < int(size)) {
    char ch = path[pos];
    if (isDirectorySeparator(ch)) {
      return pos;
    }
    ++pos;
  }
  return -1;
}

int findSeparatorRvs(const char * path, size_t size, int pos) {
  pos = std::min(pos, int(size));
  while (pos > 0) {
    char ch = path[--pos];
    if (isDirectorySeparator(ch)) {
      return pos;
    }
  }
  return -1;
}

int findExtension(const char * path, size_t size) {
  int pos = int(size);
  while (pos > 0) {
    char ch = path[--pos];
    if (ch == '.') {
      return pos + 1;
    }
    if (isDirectorySeparator(ch)) {
      return -1;
    }
  }
  return -1;
}

unsigned findNameEnd(const char * path, size_t size, int pos) {
  int result = findSeparatorFwd(path, size, pos);
  return result >= 0 ? unsigned(result) : size;
}

bool hasTrailingSeparator(const char * path, size_t size) {
  return size > 0 && isDirectorySeparator(path[size - 1]);
}

int findRoot(StringRef path) {
  // TODO: Windows drive letters.
  if (path.startsWith(DIRSEP)) {
    return DIRSEP.size();
  }
  return 0;
}

int findRoot(const char * path, size_t size) {
  return findRoot(StringRef(path, size));
}

// def makeAbsolute(path:String) -> String;
// def exists(path:String) -> bool;

void normalize(SmallVectorImpl<char> & path) {
  int end = path.size();
  int root = findRoot(path.data(), end);

  // Now copy the directory components after the root.
  int head = root; // Length of path prefix that can't be removed via '..'.
  int rpos = root;
  int wpos = head;
  while (rpos < end) {
    // Find the next dir sep or end of the path.
    int next = findSeparatorFwd(path.data(), end, rpos);
    if (next < 0) {
      next = end;
    }
    bool isSingleDot = next == rpos + 1 && path[rpos] == '.';
    bool isDoubleDot = next == rpos + 2 && path[rpos] == '.' && path[rpos + 1] == '.';
    if (isSingleDot) {
      // It's a current-dir component, so just skip over it.
    } else if (isDoubleDot && wpos > head) {
      // It's a parent-dir component. Delete the previous dir component, unless there
      // is nothing to delete.
      int prevSep = wpos - 1;
      while (prevSep > root && !isDirectorySeparator(path[prevSep])) {
        --prevSep;
      }
      wpos = prevSep;
    } else {
      // Append a directory separator.
      if (wpos > root) {
        path[wpos++] = DIRECTORY_SEPARATOR_CHAR;
      }

      // And the path component
      while (rpos < next) {
        path[wpos++] = path[rpos++];
      }

      if (isDoubleDot) {
        head = rpos;
      }
    }

    // Skip to the next character past the directory separator.
    rpos = next + 1;
  }

  path.resize(wpos);
}

// def isReadable(path:String) -> bool;
// def isWritable(path:String) -> bool;

bool isAbsolute(StringRef path) {
  return findRoot(path) != 0;
}

// def isDirectory(path:String) -> bool;

StringRef filename(StringRef path) {
  int sep = findSeparatorRvs(path);
  if (sep >= 0) {
    return path.substr(sep + 1);
  } else {
    return path;
  }
}

StringRef parent(StringRef path) {
  int sep = findSeparatorRvs(path);
  if (sep == 0) {
    return DIRSEP;
  } else if (sep > 0) {
    return path.substr(0, sep);
  } else {
    return "";
  }
}

std::pair<StringRef, StringRef> split(StringRef path) {
  int sep = findSeparatorRvs(path);
  if (sep >= 0) {
    return std::make_pair(path.substr(0, sep), path.substr(sep + 1));
  } else {
    return std::make_pair(StringRef(), path);
  }
}

StringRef extension(StringRef path) {
  int ext = findExtension(path);
  return (ext < 0) ? StringRef() : path.substr(ext);
}

void changeExtension(SmallVectorImpl<char> & path, StringRef ext) {
  int pos = findExtension(path.data(), path.size());
  if (ext.empty()) {
    // If ext is empty, then remove the extension.
    if (pos > 0) {
      path.erase(path.begin() + pos - 1, path.end());
    }
  } else {
    if (pos < 0) {
      path.push_back('.');
    } else {
      path.erase(path.begin() + pos, path.end());
    }
    append(path, ext);
  }
}

void concat(SmallVectorImpl<char> & path, StringRef newpath) {
  if (isAbsolute(newpath)) {
    assign(path, newpath);
  } else if (hasTrailingSeparator(path.data(), path.size())) {
    append(path, newpath);
  } else {
    path.push_back(DIRECTORY_SEPARATOR_CHAR);
    append(path, newpath);
  }
}

void combine(SmallVectorImpl<char> & path, StringRef newpath) {
  concat(path, newpath);
  normalize(path);
}

bool makeRelative(StringRef base, StringRef path, SmallVectorImpl<char> & result) {
  M_ASSERT(isAbsolute(path));
  M_ASSERT(isAbsolute(base));

  int rootPathPos = findRoot(path);
  int rootBasePos = findRoot(base);
  if (rootPathPos > 0 || rootBasePos > 0) {
    if (path.substr(0, rootPathPos) != base.substr(0, rootBasePos)) {
      result.append(path.begin(), path.end());
      return false;
    }
  }

  unsigned pathPos = rootPathPos;
  unsigned basePos = pathPos;
  while (pathPos < path.size() && basePos < base.size()) {
    unsigned pathSep = findNameEnd(path, pathPos);
    unsigned baseSep = findNameEnd(base, basePos);
    if (pathSep != baseSep ||
        path.substr(pathPos, pathSep - pathPos) != base.substr(basePos, baseSep - basePos)) {
      break;
    }
    pathPos = pathSep;
    basePos = baseSep;
    if (pathPos < path.size()) { ++pathPos; }
    if (basePos < base.size()) { ++basePos; }
  }

  result.clear();
  while (basePos < base.size()) {
    unsigned sep = findNameEnd(base, basePos);
    if (!result.empty()) {
      result.push_back('/');
    }
    result.push_back('.');
    result.push_back('.');
    basePos = sep + 1;
  }

  if (pathPos < path.size()) {
    if (!result.empty()) {
      result.push_back('/');
    }
    result.append(path.begin() + pathPos, path.end());
  }
  if (result.empty()) {
    result.push_back('.');
  }

  return true;
}

void toNative(StringRef in, SmallVectorImpl<native_char_t> & out) {
  #if defined(_WIN32)
    int size = MultiByteToWideChar(CP_UTF8, 0, in.data(), in.size(), NULL, 0);
    out.resize(size + 1);
    size = MultiByteToWideChar(CP_UTF8, 0, in.data(), in.size(), out.data(), out.size());
    out[size] = '\0';
    out.resize(size);
    for (SmallVectorImpl<wchar_t>::iterator
        it = out.begin(), itEnd = out.end(); it != itEnd; ++it) {
      if (*it == '/') {
        *it = '\\';
      }
    }
  #else
    out.resize(in.size() + 1);
    std::copy(in.begin(), in.end(), out.begin());
    out[in.size()] = '\0';
    out.resize(in.size());
  #endif
}

void fromNative(ArrayRef<native_char_t> in, SmallVectorImpl<char> & out) {
  #if defined(_WIN32)
    int size = ::WideCharToMultiByte(CP_UTF8, 0, in.data(), in.size(), NULL, 0, NULL, NULL);
    out.resize(size);
    size = ::WideCharToMultiByte(CP_UTF8, 0,
        in.data(), in.size(),
        out.data(), out.size(), NULL, NULL);
    for (SmallVectorImpl<char>::iterator
        it = out.begin(), itEnd = out.end(); it != itEnd; ++it) {
      if (*it == '\\') {
        *it = '/';
      }
    }
  #else
    M_ASSERT(false) << "Implement";
    out.resize(in.size());
    std::copy(in.begin(), in.end(), out.begin());
  #endif
}

void getCurrentDir(SmallVectorImpl<char> & path) {
  #if defined(_WIN32)
    DWORD length = ::GetFullPathNameW(L".", 0, NULL, NULL);
    SmallVector<wchar_t, 64> buf;
    buf.resize(length + 1); // Leave room for the terminating null.
    length = ::GetFullPathNameW(L".", length, buf.data(), NULL);
    buf.resize(length);
    fromNative(buf, path);
  #else
    char * buf = ::getcwd(NULL, 0);
    StringRef cwd(buf);
    path.append(cwd.begin(), cwd.end());
    free(buf);
  #endif
}

bool test(StringRef path, unsigned requirements, bool quiet) {
  // Create a null-terminated version of the path
  SmallString<128> pathBuffer(path);
  pathBuffer.push_back('\0');
  using namespace mint::console;

  #if HAVE_ACCESS
    if (requirements & IS_READABLE) {
      #if HAVE_ACCESS
        if (::access(pathBuffer.data(), R_OK) != 0) {
          int error = errno;
          if (error == ENOENT) {
            if (!quiet) {
              err() << "Error opening '" << path << "' for reading: no such file or directory.\n";
            }
          } else if (error == EACCES) {
            if (!quiet) {
              err() << "Error opening '" << path << "' for reading: permission denied.\n";
            }
          } else {
            err() << "Read Not OK\n";
            printPosixFileError("opening", path, errno);
          }
          return false;
        }
      #endif
    }

    if (requirements & IS_WRITABLE) {
      #if HAVE_ACCESS
        if (::access(pathBuffer.data(), W_OK) != 0) {
          int error = errno;
          if (error == ENOENT) {
            if (!quiet) {
              err() << "Error opening '" << path << "' for writing: no such file or directory.\n";
            }
          } else if (error == EACCES) {
            if (!quiet) {
              err() << "Error opening '" << path << "' for writing: permission denied.\n";
            }
          } else {
            printPosixFileError("opening", path, error);
          }
          return false;
        }
      #endif
    }

    if (requirements & IS_SEARCHABLE) {
      #if HAVE_ACCESS && !defined(_WIN32)
        if (::access(pathBuffer.data(), X_OK) != 0) {
          int error = errno;
          if (error == ENOENT) {
            if (!quiet) {
              err() << "Error accessing '" << path << "': no such file or directory.\n";
            }
          } else if (error == EACCES) {
            if (!quiet) {
              err() << "Error accessing '" << path << "': permission denied.\n";
            }
          } else {
            printPosixFileError("accessing", path, error);
          }
          return false;
        }
      #endif
    }
  #else
    #error "file access test: unimplemented"
  #endif

  #if HAVE_STAT
    if (requirements & (IS_FILE|IS_DIRECTORY)) {
      struct stat st;
      if (::stat(pathBuffer.data(), &st) != 0) {
        if (!quiet) {
          printPosixFileError("accessing", path, errno);
        }
        return false;
      }
      if (requirements & IS_FILE) {
        if ((st.st_mode & S_IFREG) == 0) {
          if (!quiet) {
            console::err() << "Error: '" << path << "' is not a file.\n";
          }
          return false;
        }
      }
      if (requirements & IS_DIRECTORY) {
        if ((st.st_mode & S_IFDIR) == 0) {
          if (!quiet) {
            console::err() << "Error: '" << path << "' is not a directory.\n";
          }
          return false;
        }
      }
    }
  #else
    #error "file mode test: unimplemented"
  #endif

  return true;
}

#if 0
unsigned fileSize(StringRef path) {
  // Create a null-terminated version of the path
  SmallVector<char, 128> pathBuffer(path.begin(), path.end());
  pathBuffer.push_back('\0');
  using namespace mint::console;

  #if HAVE_STAT
    struct stat st;
    if (::stat(pathBuffer.data(), &st) != 0) {
      printPosixFileError(path, errno);
      return 0;
    }
    if ((st.st_mode & S_IFREG) == 0) {
      err() << "Error: '" << path << "' is not a file.\n";
      return false;
    }

    return unsigned(st.st_size);
  #else
    #error "file size: unimplemented"
  #endif
}
#endif

bool fileStatus(StringRef path, FileStatus & status) {
  // Create a null-terminated version of the path
  SmallString<128> pathBuffer(path.begin(), path.end());
  pathBuffer.push_back('\0');
  using namespace mint::console;

  #if HAVE_STAT
    struct stat st;
    if (::stat(pathBuffer.data(), &st) != 0) {
      int error = errno;
      if (error == ENOENT) {
        status.exists = false;
        status.size = 0;
        return true;
      }
      printPosixFileError("accessing", path, error);
      return false;
    }

    status.exists = true;
    status.isFile = ((st.st_mode & S_IFREG) != 0);
    status.isDir = ((st.st_mode & S_IFDIR) != 0);
    //status.lastModified = st.st_mtimespec;
    status.lastModified = st.st_mtime;
    status.size = st.st_size;
    return true;
  #else
    #error "fileStatus: unimplemented"
  #endif
}

#if defined(_WIN32)
bool readFileContents(StringRef path, SmallVectorImpl<char> & buffer) {
  // Create a null-terminated version of the path
  SmallVector<native_char_t, 128> pathBuffer;
  toNative(path, pathBuffer);
  using namespace mint::console;

  HANDLE fd = ::CreateFile(pathBuffer.data(), GENERIC_READ, FILE_SHARE_READ,
    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (fd == INVALID_HANDLE_VALUE) {
    DWORD error = ::GetLastError();
    if (error == ERROR_FILE_NOT_FOUND) {
      err() << "Error opening '" << path << "' for reading: file not found.\n";
      return false;
    } else if (error == ERROR_ACCESS_DENIED) {
      err() << "Error opening '" << path << "' for reading: permission denied.\n";
      return false;
    } else {
      printWin32FileError("reading", path, error);
      return false;
    }
  }

  // Determine the size of the file
  DWORD size = ::GetFileSize(fd, NULL);
  if (size == INVALID_FILE_SIZE) {
    printWin32FileError("reading", path, ::GetLastError());
    ::CloseHandle(fd);
    return false;
  }

  // Allocate a buffer to hold the entire file, and read the file into it.
  buffer.resize(unsigned(size));
  DWORD actual;
  if (!::ReadFile(fd, buffer.data(), DWORD(size), &actual, NULL)) {
    printWin32FileError("reading", path, ::GetLastError());
    ::CloseHandle(fd);
    return false;
  }
  M_ASSERT(actual == size);

  ::CloseHandle(fd);
  return true;
}
#else
bool readFileContents(StringRef path, SmallVectorImpl<char> & buffer) {
  int fd = openFileForRead(path);
  if (fd == -1) {
    return false;
  }

  // Determine the size of the file
  off_t size = ::lseek(fd, 0, SEEK_END);
  if (size == -1) {
    printPosixFileError("reading", path, errno);
    ::close(fd);
    return false;
  }
  ::lseek(fd, 0, SEEK_SET);

  // Allocate a buffer to hold the entire file, and read the file into it.
  buffer.resize(unsigned(size));
  ssize_t actual = ::read(fd, buffer.data(), unsigned(size));
  if (actual == -1) {
    printPosixFileError("reading", path, errno);
    ::close(fd);
    return false;
  }

  ::close(fd);
  return true;
}
#endif

#if defined(_WIN32)
bool writeFileContents(StringRef path, StringRef content) {
  StringRef parentDir = parent(path);
  if (!parentDir.empty()) {
    if (!makeDirectoryPath(parentDir)) {
      return false;
    }
  }

  SmallVector<native_char_t, 128> pathBuffer;
  toNative(path, pathBuffer);
  HANDLE fd = ::CreateFile(pathBuffer.data(), GENERIC_WRITE, FILE_SHARE_WRITE,
    NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (fd == INVALID_HANDLE_VALUE) {
    DWORD error = ::GetLastError();
    if (error == ERROR_ACCESS_DENIED) {
      console::err() << "Error opening '" << path << "' for writing: permission denied.\n";
    } else {
      printWin32FileError("writing", path, error);
    }
    return false;
  }

  for (;;) {
    if (!::WriteFile(fd, content.data(), content.size(), NULL, NULL)) {
      printWin32FileError("writing", path, ::GetLastError());
      ::CloseHandle(fd);
      return false;
    }
    break;
  }

  ::CloseHandle(fd);
  return true;
}
#else
bool writeFileContents(StringRef path, StringRef content) {
  StringRef parentDir = parent(path);
  if (!parentDir.empty()) {
    if (!makeDirectoryPath(parentDir)) {
      return false;
    }
  }

  int fd = openFileForWrite(path);
  if (fd == -1) {
    return false;
  }
  for (;;) {
    ssize_t result = ::write(fd, content.data(), content.size());
    if (result < 0) {
      int error = errno;
      if (error == EINTR || error == EAGAIN) {
        continue;
      } else {
        printPosixFileError("writing", path, error);
        ::close(fd);
        return false;
      }
    }
    break;
  }

  ::close(fd);
  return true;
}
#endif

#if defined(_WIN32)
bool copyFile(StringRef sourcePath, StringRef outputPath) {
  SmallVector<native_char_t, 128> sourcePathBuffer;
  SmallVector<native_char_t, 128> outputPathBuffer;
  toNative(sourcePath, sourcePathBuffer);
  toNative(outputPath, outputPathBuffer);
  if (!::CopyFile(sourcePathBuffer.data(), outputPathBuffer.data(), false)) {
    printWin32FileError("copying to", outputPath, ::GetLastError());
    return false;
  }
  return true;
}
#else
bool copyFile(StringRef sourcePath, StringRef outputPath) {
  int rfd = openFileForRead(sourcePath);
  if (rfd == -1) {
    return false;
  }

  StringRef parentDir = parent(outputPath);
  if (!parentDir.empty()) {
    if (!makeDirectoryPath(parentDir)) {
      return false;
    }
  }

  int wfd = openFileForWrite(outputPath);
  if (wfd == -1) {
    ::close(rfd);
    return false;
  }

  char buf[4096];
  for (;;) {
    ssize_t nread = ::read(rfd, buf, sizeof(buf));
    if (nread > 0) {
      char * out = buf;
      do {
        // Attempt to write all that was read, but we might get interrupted.
        // (TODO: Modify the other functions here to do this as well.)
        ssize_t written = ::write(wfd, out, size_t(nread));
        if (written >= 0) {
          out += written;
          nread -= written;
        } else if (errno != EINTR) {
          printPosixFileError("writing", outputPath, errno);
          goto done;
        }
      } while (nread > 0);
    } else if (nread < 0) {
      if (errno != EINTR) {
        printPosixFileError("reading", sourcePath, errno);
        goto done;
      }
    } else {
      // Done.
      break;
    }
  }

done:
  ::close(wfd);
  ::close(rfd);
  return true;
}
#endif

bool writeFileContentsIfDifferent(StringRef path, StringRef newContent) {
  FileStatus st;
  bool changed = false;
  if (!fileStatus(path, st)) {
    changed = true;
  } else if (!st.exists) {
    changed = true;
  } else {
    SmallString<0> oldContent;
    if (!readFileContents(path, oldContent)) {
      changed = true;
    } else if (oldContent != newContent) {
      changed = true;
    }
  }

  if (changed) {
    return writeFileContents(path, newContent);
  }
  return true;
}

bool makeDirectoryPath(StringRef path) {
  // If it's a root, just return that it exists.
  if (path.empty() || findRoot(path) >= int(path.size())) {
    return true;
  }

  // Create a null-terminated version of the path
  SmallVector<native_char_t, 128> pathBuffer;
  toNative(path, pathBuffer);

  #if HAVE_STAT
    #if defined(_WIN32)
      struct _stat64i32 st;
      if (::_wstat(pathBuffer.data(), &st) != 0) {
    #else
      struct stat st;
      if (::stat(pathBuffer.data(), &st) != 0) {
    #endif
      int error = errno;
      if (error == ENOENT || error == ENOTDIR) {
        if (!makeDirectoryPath(parent(path))) {
          return false;
        }
        #if defined(_WIN32)
          if (::_wmkdir(pathBuffer.data()) == 0) {
        #else
          if (::mkdir(pathBuffer.data(), S_IRUSR | S_IWUSR | S_IXUSR) == 0) {
        #endif
          return true;
        }
        error = errno;
      }
      printPosixFileError("accessing", path, error);
      return false;
    }

    if ((st.st_mode & S_IFDIR) == 0) {
      console::err() << "Error: '" << path << "' is not a directory.\n";
      return false;
    }
    return true;
  #else
    #error "makeDirectoryPath: unimplemented for this platform"
  #endif
}

bool remove(StringRef path) {
  // Create a null-terminated, native version of the path
  SmallVector<native_char_t, 128> pathBuffer;
  toNative(path, pathBuffer);
  #if defined(_WIN32)
    int status = ::_wunlink(pathBuffer.data());
  #elif HAVE_UNISTD_H
    int status = ::unlink(pathBuffer.data());
  #else
    #error Unimplemented: File::remove();
  #endif
    if (status == -1) {
      int error = errno;
      if (error == ENOENT) {
        return true;
      }
      printPosixFileError("removing", path, error);
      return false;
    }
  return true;
}

}}
