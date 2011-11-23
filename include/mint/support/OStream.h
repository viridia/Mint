/* ================================================================== *
 * Generic output streams
 * ================================================================== */

#ifndef MINT_SUPPORT_OSTREAM_H
#define MINT_SUPPORT_OSTREAM_H

#ifndef MINT_CONFIG_H
#include "mint/config.h"
#endif

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLVECTOR_H
#include "mint/collections/SmallVector.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    A generic output stream class. Inspired by LLVM's OStream class.
 */
class OStream {
public:
  // color order matches ANSI escape sequence, don't change
  enum Colors {
    BLACK=0,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    SAVEDCOLOR
  };

  OStream() {}

  OStream & operator<<(char ch) {
//    if (OutBufCur >= OutBufEnd)
      return write(ch);
//    *OutBufCur++ = C;
    return *this;
  }

  OStream & operator<<(unsigned char ch) {
//    if (OutBufCur >= OutBufEnd)
      return write(ch);
//    *OutBufCur++ = C;
    return *this;
  }

  OStream & operator<<(signed char ch) {
//    if (OutBufCur >= OutBufEnd)
      return write(ch);
//    *OutBufCur++ = C;
    return *this;
  }

  OStream & operator<<(StringRef str) {
    // Inline fast path, particularly for strings with a known length.
    //size_t size = str.size();

    // Make sure we can use the fast path.
//    if (OutBufCur + size > OutBufEnd)
      return write(str.data(), str.size());

//    memcpy(OutBufCur, str.data(), size);
//    OutBufCur += size;
    return *this;
  }

  OStream & operator<<(const char * str) {
    // Inline fast path, particularly for constant strings where a sufficiently
    // smart compiler will simplify strlen.

    return this->operator<<(StringRef(str));
  }

//  OStream & operator<<(const std::string & str) {
//    // Avoid the fast path, it would only increase code size for a marginal win.
//    return write(str.data(), str.length());
//  }

  OStream & operator<<(unsigned long n);
  OStream & operator<<(long n);
  OStream & operator<<(unsigned long long n);
  OStream & operator<<(long long n);
//  OStream & operator<<(const void * ptr);
  OStream & operator<<(unsigned int n) {
    return this->operator<<(static_cast<unsigned long>(n));
  }

  OStream & operator<<(int n) {
    return this->operator<<(static_cast<long>(n));
  }

  OStream & operator<<(double n);

  OStream & write(unsigned char ch);
  OStream & write(const char * ptr, size_t size);

  // Formatted output, see the format() function in Support/Format.h.
  //OStream & operator<<(const format_object_base & Fmt);

  /// write 'numSpaces' spaces.
  OStream & indent(unsigned numSpaces);

  /** Changes the color of text that will be output from this point forward.
      Parameters:
        colors: ANSI color to use, the special SAVEDCOLOR can be used to change only the
          bold attribute, and keep colors untouched.
        bold: if true, change to bold, otherwise change to non-bold.
        bg: if true change the background, otherwise change foreground
   */
  virtual OStream & changeColor(enum Colors, bool bold = false, bool bg = false) {
    return *this;
  }

  /** Resets the colors to terminal defaults. Call this when you are done
      outputting colored text, or before program exit.
   */
  virtual OStream & resetColor() { return *this; }

  /** This function determines if this stream is connected to a "tty" or
     "console" window. That is, the output would be displayed to the user
      rather than being put on a pipe or stored in a file. */
  virtual bool isTerminal() const { return false; }
protected:
  /** Subclasses should override this. */
  virtual void writeImpl(const char * buf, size_t length) = 0;

private:
  // Do not implement. OStream is non-copyable.
  void operator=(const OStream &);
  OStream(const OStream &);
};

/** -------------------------------------------------------------------------
    OStream that writes to a text buffer.
 */
class OStrStream : public OStream {
public:
  OStrStream() {}

  StringRef str() const;

private:
  // Do not implement. OStrStream is non-copyable.
  void operator=(const OStrStream &);
  OStrStream(const OStrStream &);

  void writeImpl(const char * buf, size_t length);

  SmallVector<char, 32> _str;
};

/** -------------------------------------------------------------------------
    OStream that writes to a POSIX file handle.
 */
class OFileStream : public OStream {
public:
  OFileStream(StringRef fileName);
  OFileStream(int fd, bool shouldClose) : _fd(fd), _shouldClose(shouldClose) {}
  ~OFileStream();

  /// True if the stream opened successfully
  bool valid() const { return _fd != -1; }

  // Overrides

  bool isTerminal() const;
  OStream & changeColor(enum Colors, bool bold = false, bool bg = false);
  OStream & resetColor();

private:
  // Do not implement. OFileStream is non-copyable.
  void operator=(const OFileStream &);
  OFileStream(const OFileStream &);

  void writeImpl(const char * buf, size_t length);

  // File descriptor
  int _fd;

  // If true, close the file handle when done.
  bool _shouldClose;
};

namespace console {
  /** Standard error stream. */
  OStream & err();

  /** Standard output stream. */
  OStream & out();
}

}

#endif // MINT_SUPPORT_OSTREAM_H
