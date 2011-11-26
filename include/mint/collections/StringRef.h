/* ================================================================== *
 * StringRef
 * ================================================================== */

#ifndef MINT_COLLECTIONS_STRINGREF_H
#define MINT_COLLECTIONS_STRINGREF_H

#ifndef MINT_COLLECTIONS_SMALLVECTOR_H
#include "mint/collections/SmallVector.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

#if HAVE_ITERATOR
#include <iterator>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    A constant reference to a string. (Inspired by on LLVM's StringRef class.)
 */
class StringRef {
public:
  typedef const char * iterator;
  typedef const char * const_iterator;
  static const size_t npos = ~size_t(0);
  typedef size_t size_type;

  /// Construct an empty StringRef.
  StringRef() : _data(NULL), _size(0) {}

  /// Copy constructor.
  StringRef(const StringRef & src) : _data(src._data), _size(src._size) {}

  /// Construct a StringRef from a cstring.
  StringRef(const char * str) : _data(str) {
    M_ASSERT_BASE(str && "StringRef cannot be built from a NULL argument");
    _size = ::strlen(str);
  }

  /// Construct a StringRef from a pointer and length.
  StringRef(const char * str, size_t length) : _data(str), _size(length) {}

  /// Construct a StringRef from a character vector.
  StringRef(const SmallVectorImpl<char> & vec) : _data(vec.data()), _size(vec.size()) {}

  // Iterators

  iterator begin() const { return _data; }
  iterator end() const { return _data + _size; }

  // String operations

  /// Pointer to the raw string data, which may not be null terminated.
  const char * data() const { return _data; }

  /// Length of the string in bytes.
  size_t size() const { return _size; }

  /// True if size == 0.
  bool empty() const { return _size == 0; }

  /// Array element operator.
  char operator[](size_t index) const {
    M_ASSERT_BASE(index < _size && "Invalid index!");
    return _data[index];
  }

  /// substr - return a reference to the substring from [Start, Start + N).
  StringRef substr(size_t start, size_t n = npos) const {
    start = std::min(start, _size);
    return StringRef(_data + start, std::min(n, _size - start));
  }

  /// Check for string equality.
  bool equals(StringRef rhs) const {
    return (_size == rhs._size && compareMemory(_data, rhs._data, rhs._size) == 0);
  }

  /// Compare two strings for lexicographical ordering.
  bool compare(StringRef rhs) const {
    if (int result = compareMemory(_data, rhs._data, std::min(_size, rhs._size))) {
      return result < 0 ? -1 : 1;
    }
    if (_size == rhs._size) { return 0; }
    return _size < rhs._size ? -1 : 1;
  }

  /// Check for string prefix.
  bool startsWith(StringRef rhs) const {
    return (_size >= rhs._size && compareMemory(_data, rhs._data, rhs._size) == 0);
  }

  /// Find the first occurrence of character 'ch'
  size_t find(char ch, size_t from = 0) {
    from = std::min(from, _size);
    for (size_t i = from; i < _size; ++i) {
      if (_data[i] == ch) {
        return i;
      }
    }
    return npos;
  }

  /// Find the last occurrence of character 'ch'
  size_t rfind(char ch, size_t from = npos) {
    from = std::min(from, _size);
    for (size_t i = from; i != 0;) {
      if (_data[--i] == ch) {
        return i;
      }
    }
    return npos;
  }

  /// Hashing function
  unsigned hash() const;

private:
  // Workaround memcmp issue with null pointers (undefined behavior)
  // by providing a specialized version
  static int compareMemory(const char * lhs, const char * rhs, size_t length) {
    if (length == 0) { return 0; }
    return ::memcmp(lhs, rhs, length);
  }

  /// The start of the string, in an external buffer.
  const char * _data;

  /// The length of the string.
  size_t _size;
};

// Comparison Operators

inline bool operator==(StringRef lhs, StringRef rhs) {
  return lhs.equals(rhs);
}

inline bool operator!=(StringRef lhs, StringRef rhs) {
  return !(lhs == rhs);
}

}

#endif // MINT_COLLECTIONS_STRINGREF_H
