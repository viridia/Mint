/* ================================================================== *
 * SmallString class.
 * ================================================================== */

#ifndef MINT_COLLECTIONS_SMALLSTRING_H
#define MINT_COLLECTIONS_SMALLSTRING_H

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

namespace mint {

template<int N>
class SmallString : public SmallVector<char, N> {
public:
  typedef const char * const_iterator;
  typedef const char * const_pointer;
  static const size_t npos = ~size_t(0);

  /// Default constructor
  SmallString() : SmallVector<char, N>() {}

  /// Constructor from an iterator pair
  SmallString(const_iterator first, const_iterator last) : SmallVector<char, N>(first, last) {}

  /// Constructor from a SmallVector
  explicit SmallString(const SmallVectorImpl<char> & rhs)
    : SmallVector<char, N>(rhs.begin(), rhs.end()) {}

  /// Constructor from a StringRef
  explicit SmallString(StringRef sr) : SmallVector<char, N>(sr.begin(), sr.end()) {}

  /// Conversion operator to StringRef
  operator StringRef() const { return StringRef(this->data(), this->size()); }

  /// Assignment from a SmallVector
  const SmallString & operator=(const SmallVectorImpl<char> & rhs) {
    SmallVectorImpl<char>::operator=(rhs);
    return *this;
  }

  /// Assignment from a StringRef
  const SmallString & operator=(StringRef rhs) {
    assign(rhs.begin(), rhs.end());
    return *this;
  }

  /// Append from a SmallVector
  const SmallString & operator+=(const SmallVectorImpl<char> & rhs) {
    SmallVectorImpl<char>::append(rhs.begin(), rhs.end());
    return *this;
  }

  /// Assignment from a StringRef
  const SmallString & operator+=(StringRef rhs) {
    SmallVectorImpl<char>::append(rhs.begin(), rhs.end());
    return *this;
  }

  /// Append from an iterator pair
  void append(const_iterator first, const_iterator last) {
    SmallVectorImpl<char>::append(first, last);
  }

  /// Append from a StringRef
  void append(StringRef rhs) {
    SmallVectorImpl<char>::append(rhs.begin(), rhs.end());
  }

  /// Append from a SmallVector
  void append(const SmallVectorImpl<char> & rhs) {
    SmallVectorImpl<char>::append(rhs.begin(), rhs.end());
  }

  /// Assign from an iterator pair
  void assign(const_iterator first, const_iterator last) {
    this->clear();
    SmallVectorImpl<char>::append(first, last);
  }

  /// Assign from a StringRef
  void assign(StringRef rhs) {
    this->clear();
    SmallVectorImpl<char>::append(rhs.begin(), rhs.end());
  }

  /// Assign from a SmallVector
  void assign(const SmallVectorImpl<char> & rhs) {
    this->clear();
    SmallVectorImpl<char>::append(rhs.begin(), rhs.end());
  }

  /// substr - return a reference to the substring from [Start, Start + N).
  StringRef substr(size_t start, size_t n = npos) const {
    start = std::min(start, this->size());
    return StringRef(this->data() + start, std::min(n, this->size() - start));
  }

  /// Return the contents as a null-terminated string.
  char * cstr() {
    this->reserve(size() + 1);
    this->data()[size()] = '\0';
    return this->data();
  }
};

}

#endif // MINT_COLLECTIONS_SMALLSTRING_H
