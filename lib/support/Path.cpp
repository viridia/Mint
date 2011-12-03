/* ================================================================ *
   Path manipulation functions
 * ================================================================ */

#include "mint/collections/SmallString.h"

#include "mint/support/Assert.h"
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
  if (pos < 0) {
    path.push_back('.');
  } else {
    path.erase(path.begin() + pos, path.end());
  }
  append(path, ext);
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

// def toNative(path:String) -> String;
// def fromNative(path:String) -> String;

void getCurrentDir(SmallVectorImpl<char> & path) {
  char * buf = ::getcwd(NULL, 0);
  StringRef cwd(buf);
  path.append(cwd.begin(), cwd.end());
  free(buf);
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
      #if HAVE_ACCESS
        if (::access(pathBuffer.data(), X_OK) != 0) {
          int error = errno;
          if (error == ENOENT) {
            err() << "Error accessing '" << path << "': no such file or directory.\n";
          } else if (error == EACCES) {
            err() << "Error accessing '" << path << "': permission denied.\n";
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

bool readFileContents(StringRef path, SmallVectorImpl<char> & buffer) {
  // Create a null-terminated version of the path
  SmallString<128> pathBuffer(path.begin(), path.end());
  pathBuffer.push_back('\0');
  using namespace mint::console;

  int fd = ::open(pathBuffer.data(), O_RDONLY);
  if (fd == -1) {
    int error = errno;
    if (error == ENOENT) {
      err() << "Error opening '" << path << "' for reading: file not found.\n";
      return false;
    } else if (error == EACCES) {
      err() << "Error opening '" << path << "' for reading: permission denied.\n";
      return false;
    } else {
      printPosixFileError("reading", path, error);
      return false;
    }
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

bool writeFileContents(StringRef path, StringRef content) {
  StringRef parentDir = parent(path);
  if (!parentDir.empty()) {
    if (!makeDirectoryPath(parentDir)) {
      return false;
    }
  }

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

bool makeDirectoryPath(StringRef path) {
  // If it's a root, just return that it exists.
  if (path.empty() || findRoot(path) >= int(path.size())) {
    return true;
  }

  // Create a null-terminated version of the path
  SmallString<128> pathBuffer(path);
  pathBuffer.push_back('\0');

  #if HAVE_STAT
    struct stat st;
    if (::stat(pathBuffer.data(), &st) != 0) {
      int error = errno;
      if (error == ENOENT || error == ENOTDIR) {
        if (!makeDirectoryPath(parent(path))) {
          return false;
        }
        if (::mkdir(pathBuffer.data(), S_IRUSR | S_IWUSR | S_IXUSR) == 0) {
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

}}
