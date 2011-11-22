/* ================================================================ *
   Represents a buffer containing lines of text.
 * ================================================================ */

#ifndef MINT_SUPPORT_TEXTBUFFER_H
#define MINT_SUPPORT_TEXTBUFFER_H

#ifndef MINT_COLLECTIONS_SMALLSTRING_H
#include "mint/collections/SmallString.h"
#endif

#ifndef MINT_SUPPORT_REFCOUNTABLE_H
#include "mint/support/RefCountable.h"
#endif

#if HAVE_ALGORITHM
#include <algorithm>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    A buffer representing a parseable text file.
 */
class TextBuffer : public RefCountable {
public:
  typedef const char * iterator;
  typedef const char * const_iterator;

  /// Constructor
  TextBuffer() {}
  TextBuffer(const char * buffer, unsigned size) : _buffer(StringRef(buffer, size)) {}
  TextBuffer(StringRef str) : _buffer(str) {}

  /// The character buffer
  const SmallVectorImpl<char> & buffer() const { return _buffer; }
  SmallVectorImpl<char> & buffer() { return _buffer; }

  // Iterators

  const_iterator begin() const { return _buffer.begin(); };
  const_iterator end() const { return _buffer.end(); };

  // Accessors

  unsigned size() const { return _buffer.size(); }

  // The array of line break positions.

  const SmallVectorImpl<unsigned> & lines() const { return _lines; }

  /// The file path from which this buffer was read
  StringRef filePath() const { return _filePath; }
  void setFilePath(StringRef str) { _filePath = str; }

  /// Append a line break at a given offset to the line break table.
  void lineBreak(unsigned offset, bool force = false) {
    /// In some cases (where an error has been reported) the entries in the line table may
    /// run ahead of the current parsing position - in which case, don't record anything.
    if (force || offset > lastLineEnd()) {
      _lines.push_back(offset);
    }
  }
  void lineBreak(const_iterator pos) { lineBreak(unsigned(pos - _buffer.begin())); }

  /// Return the index of the line that contains the given offset
  unsigned findContainingLine(unsigned offset) const {
    // If we've advanced past the end of the last recorded line, then find the end of
    // the current line.
    if (lastLineEnd() <= offset) {
      const_cast<TextBuffer *>(this)->findLineBreaks(offset);
    }
    SmallVectorImpl<unsigned>::const_iterator it = std::upper_bound(
        _lines.begin(), _lines.end(), offset) - 1;
    return it - _lines.begin();
  }

private:
  /// Return the ending position of the last recorded line.
  unsigned lastLineEnd() const {
    return _lines.empty() ? 0 : _lines.back();
  }

  // Find all of the line breaks up to the end of the line containing 'offset'.
  void findLineBreaks(unsigned offset) {
    unsigned index = lastLineEnd();
    while (index < _buffer.size()) {
      char ch = _buffer[index++];
      if (ch == '\n') {
        lineBreak(index);
        if (index > offset) {
          break;
        }
      } else if (ch == '\r') {
        if (index < _buffer.size() && _buffer[index] == '\n') {
          ++index;
        }
        lineBreak(index);
        if (index > offset) {
          break;
        }
      }
    }

    if (index == _buffer.size()) {
      lineBreak(index);
    }
  }

  SmallString<0> _buffer;
  SmallVector<unsigned, 0> _lines;
  StringRef _filePath;
};

} // namespace

#endif // MINT_SUPPORT_TEXTBUFFER_H
