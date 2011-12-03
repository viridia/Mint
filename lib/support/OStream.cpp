/* ================================================================== *
 * OStream
 * ================================================================== */

#include "mint/collections/SmallString.h"
#include "mint/support/OSError.h"
#include "mint/support/OStream.h"

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

namespace mint {

#define ANSI_RESET_COLOR "\033[0m"
#define ANSI_BOLD "\033[1m"

#define ANSI_COLOR(FGBG, CODE, BOLD) "\033[0;" BOLD FGBG CODE "m"

#define ALLCOLORS(FGBG,BOLD) {\
    ANSI_COLOR(FGBG, "0", BOLD),\
    ANSI_COLOR(FGBG, "1", BOLD),\
    ANSI_COLOR(FGBG, "2", BOLD),\
    ANSI_COLOR(FGBG, "3", BOLD),\
    ANSI_COLOR(FGBG, "4", BOLD),\
    ANSI_COLOR(FGBG, "5", BOLD),\
    ANSI_COLOR(FGBG, "6", BOLD),\
    ANSI_COLOR(FGBG, "7", BOLD)\
  }

static const char colorcodes[2][2][8][10] = {
 { ALLCOLORS("3",""), ALLCOLORS("3","1;") },
 { ALLCOLORS("4",""), ALLCOLORS("4","1;") }
};

static const char * escapeCode(enum OStream::Colors color, bool bold, bool bg) {
  return colorcodes[bg?1:0][bold?1:0][color&7];
}

// -------------------------------------------------------------------------
// OStream
// -------------------------------------------------------------------------

OStream & OStream::operator<<(unsigned long n) {
  // Zero is a special case.
  if (n == 0) {
    return *this << '0';
  }

  char buf[20];
  char * end = buf + sizeof(buf);
  char * pos = end;

  while (n) {
    *--pos = '0' + char(n % 10);
    n /= 10;
  }
  writeImpl(pos, size_t(end - pos));
  return *this;
}

OStream & OStream::operator<<(long n) {
  if (n <  0) {
    *this << '-';
    // Avoid undefined behavior on LONG_MIN with a cast.
    n = -(unsigned long)n;
  }

  return this->operator<<(static_cast<unsigned long>(n));
}

OStream & OStream::operator<<(unsigned long long n) {
  // Output using 32-bit div/mod when possible.
  if (n == static_cast<unsigned long>(n))
    return this->operator<<(static_cast<unsigned long>(n));

  char buf[20];
  char *end = buf + sizeof(buf);
  char *pos = end;

  while (n) {
    *--pos = '0' + char(n % 10);
    n /= 10;
  }
  writeImpl(pos, size_t(end - pos));
  return *this;
}

OStream & OStream::operator<<(long long n) {
  if (n < 0) {
    *this << '-';
    // Avoid undefined behavior on INT64_MIN with a cast.
    n = -(unsigned long long)n;
  }

  return this->operator<<(static_cast<unsigned long long>(n));
}

OStream & OStream::operator<<(double d) {
  char buffer[32];
  size_t length = ::snprintf(&buffer[0], sizeof(buffer), "%e", d);
  writeImpl(buffer, length);
  return *this;
}

OStream & OStream::indent(unsigned numSpaces) {
  static const char spaces[] = "                                                                ";
  while (numSpaces > sizeof(spaces) - 1) {
    write(spaces, sizeof(spaces) - 1);
    numSpaces -= sizeof(spaces) - 1;
  }

  write(spaces, numSpaces);
  return *this;
}

OStream & OStream::write(unsigned char ch) {
  writeImpl((const char *)&ch, size_t(1));
  return *this;
}

OStream & OStream::write(char const * buf, size_t length) {
  writeImpl(buf, length);
  return *this;
}

// -------------------------------------------------------------------------
// OStrStream
// -------------------------------------------------------------------------

StringRef OStrStream::str() const {
  return StringRef(_str.data(), _str.size());
}

void OStrStream::writeImpl(const char * buf, size_t length) {
  _str.append(buf, buf + length);
}

// -------------------------------------------------------------------------
// OFileStream
// -------------------------------------------------------------------------

OFileStream::OFileStream(StringRef fileName)
  : _shouldClose(false)
{
  SmallString<128> path(fileName);
  path.push_back('\0');
  _fd = ::open(path.data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (_fd == -1) {
    int error = errno;
    if (error == EACCES) {
      console::err() << "Error opening '" << path << "' for writing: permission denied.\n";
    } else {
      printPosixFileError(path, error);
    }
    _shouldClose = true;
  } else {

  }
}

OFileStream::~OFileStream() {
  if (_shouldClose) {
    ::close(_fd);
  }
}

OStream & OFileStream::changeColor(enum Colors color, bool bold, bool bg) {
#if ANSI_COLORS
//  if (sys::Process::ColorNeedsFlush()) {
//    flush();
//  }
  const char * colorcode = (color == SAVEDCOLOR) ? ANSI_BOLD : escapeCode(color, bold, bg);
  if (colorcode) {
    writeImpl(colorcode, strlen(colorcode));
  }
#endif
  return *this;
}

OStream & OFileStream::resetColor() {
#if ANSI_COLORS
//  if (sys::Process::ColorNeedsFlush()) {
//    flush();
//  }
  writeImpl(ANSI_RESET_COLOR, strlen(ANSI_RESET_COLOR));
#endif
  return *this;
}

bool OFileStream::isTerminal() const {
#if HAVE_ISATTY
  return isatty(_fd);
#else
  // If we don't have isatty, just return false.
  return false;
#endif
}

void OFileStream::writeImpl(const char * buf, size_t length) {
  while (length > 0) {
    ssize_t result = ::write(_fd, buf, length);
    if (result < 0) {
      if (errno == EINTR || errno == EAGAIN) {
        continue;
      }

      // For now just quit. Do an error message later.
      console::err() << "File write failed.\n";
      abort();
    }

    buf += result;
    length -= result;
  }
}

OStream & console::err() {
  static OFileStream errStrm(STDERR_FILENO, true);
  return errStrm;
}

/** Standard output stream. */
OStream & console::out() {
  static OFileStream errStrm(STDOUT_FILENO, true);
  return errStrm;
}

}
