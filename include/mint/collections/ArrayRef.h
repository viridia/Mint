/* ================================================================== *
 * ArrayRef
 * ================================================================== */

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#define MINT_COLLECTIONS_ARRAYREF_H

#ifndef MINT_SUPPORT_ASSERTBASE_H
#include "mint/support/AssertBase.h"
#endif

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
    A constant reference to an array. (Inspired by on LLVM's ArrayRef class.)
 */
template<class T>
class ArrayRef {
public:
  typedef const T * iterator;
  typedef const T * const_iterator;
  typedef size_t size_type;

  /// Construct an empty ArrayRef.
  ArrayRef() : _data(NULL), _size(0) {}

  /// Copy constructor.
  ArrayRef(const ArrayRef & src) : _data(src._data), _size(src._size) {}

  /// Construct an ArrayRef from am iterator pair.
  ArrayRef(const T * first, const T * last) : _data(first), _size(size_type(last - first)) {}

  /// Construct an ArrayRef from a pointer and length.
  ArrayRef(const T * data, size_t size) : _data(data), _size(size) {}

  /// Construct an ArrayRef from a SmallVectorImpl.
  ArrayRef(const SmallVectorImpl<T> & sv) : _data(sv.data()), _size(sv.size()) {}

  /// Construct an ArrayRef from a SmallVector.
  template <size_t Size>
  ArrayRef(const SmallVector<T, Size> & sv) : _data(sv.data()), _size(sv.size()) {}

  /// Construct an ArrayRef from a C array.
  template <size_t Size> ArrayRef(const T (&array)[Size]) : _data(array), _size(Size) {}

  // Iterators

  iterator begin() const { return _data; }
  iterator end() const { return _data + _size; }

  // Array operations

  /// Pointer to the raw string data, which may not be null terminated.
  const T * data() const { return _data; }

  /// Length of the string in bytes.
  size_t size() const { return _size; }

  /// True if size == 0.
  bool empty() const { return _size == 0; }

  /// Array element operator.
  const T & operator[](size_t index) const {
    M_ASSERT_BASE(index < _size && "Invalid index!");
    return _data[index];
  }

  // Comparisons

  bool operator==(const ArrayRef & ar) const {
    if (_size != ar._size) {
      return false;
    }
    for (size_t i = 0; i < _size; ++i) {
      if (_data[i] != ar._data[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const ArrayRef & ar) const {
    return !(*this == ar);
  }

private:

  /// The start of the string, in an external buffer.
  const T * _data;

  /// The length of the string.
  size_t _size;
};

/// Construct an ArrayRef from a single element.
template<typename T>
inline ArrayRef<T> makeArrayRef(const T & el) {
  return ArrayRef<T>(&el, 1);
}

/// Construct an ArrayRef from a pointer range.
template<typename T>
inline ArrayRef<T> makeArrayRef(const T * first, const T * last) {
  return ArrayRef<T>(first, last);
}

/// Construct an ArrayRef from a pointer and length.
template<typename T>
inline ArrayRef<T> makeArrayRef(const T * data, size_t size) {
  return ArrayRef<T>(data, size);
}

/// Construct an ArrayRef from a SmallVector.
template<typename T>
inline ArrayRef<T> makeArrayRef(const SmallVectorImpl<T> & sv) {
  return ArrayRef<T>(sv);
}

/// Construct an ArrayRef from a C array of known length.
template<typename T, size_t Size>
inline ArrayRef<T> makeArrayRef(const T (&array)[Size]) {
  return ArrayRef<T>(array, Size);
}

}

#endif // MINT_COLLECTIONS_STRINGREF_H
