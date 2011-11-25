/* ================================================================== *
 * StringRef
 * ================================================================== */

#ifndef MINT_COLLECTIONS_SMALLVECTOR_H
#define MINT_COLLECTIONS_SMALLVECTOR_H

#ifndef MINT_SUPPORT_ASSERTBASE_H
#include "mint/support/AssertBase.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_ITERATOR
#include <iterator>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Base class of SmallVector. This class doesn't know/care about the 'N'
    parameter so it reduces code duplication. Also, SmallVectors with
    differing 'N' values can inter-operate via this class.
    (Loosely based on LLVM's SmallVectorImpl class, but this one is much
    simpler since we don't need as much flexibility.)
 */
template<typename T>
class SmallVectorImpl {
public:

  // Typedefs

  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T value_type;
  typedef T * iterator;
  typedef const T * const_iterator;

  typedef T & reference;
  typedef const T & const_reference;
  typedef T * pointer;
  typedef const T * const_pointer;

  // Default ctor - Initialize to empty.
  explicit SmallVectorImpl(unsigned initialCapacity) {
    _begin = _end = internalBuffer();
    _capacity = _begin + initialCapacity;
  }

  // Iterators

  iterator begin() { return _begin; }
  const_iterator begin() const { return _begin; }
  iterator end() { return _end; }
  const_iterator end() const { return _end; }

  // Accessors

  bool empty() const { return _begin == _end; }
  size_type size() const { return _end - _begin; }
  size_t capacity() const { return _capacity - _begin; }
  pointer data() { return _begin; }
  const_pointer data() const { return _begin; }

  reference operator[](unsigned idx) {
    M_ASSERT_BASE(begin() + idx < end());
    return begin()[idx];
  }
  const_reference operator[](unsigned idx) const {
    M_ASSERT_BASE(begin() + idx < end());
    return begin()[idx];
  }

  reference front() { return _begin[0]; }
  const_reference front() const { return _begin[0]; }

  reference back() { return _end[-1]; }
  const_reference back() const { return _end[-1]; }

  // Vector operations

  void clear() {
    this->destroy_range(_begin, _end);
    _end = _begin;
  }

  void resize(unsigned n) {
    if (n < size()) {
      this->destroy_range(_begin + n, _end);
      _end = _begin + n;
    } else if (n > this->size()) {
      if (this->capacity() < n) {
        this->grow(n);
      }
      this->construct_range(_end, _begin + n, T());
      _end = _begin + n;
    }
  }

  void resize(unsigned n, const value_type & newValue) {
    if (n < this->size()) {
      this->destroy_range(_begin+n, _end);
      _end = _begin + n;
    } else if (n > this->size()) {
      if (this->capacity() < n) {
        this->grow(n);
      }
      construct_range(_end, _begin + n, newValue);
      _end = _begin + n;
    }
  }

  void reserve(unsigned n) {
    if (this->capacity() < n) {
      this->grow(n);
    }
  }

  void push_back(const value_type & value) {
    if (this->_end >= this->_capacity) {
      this->grow();
    }
    new (_end) T(value);
    ++_end;
  }

  void pop_back() {
    (--_end)->~T();
  }

  template<typename InputIterTy>
  void append(InputIterTy first, InputIterTy last) {
    size_type numValues = std::distance(first, last);
    if (numValues > size_type(_capacity - _end)) {
      this->grow(this->size() + numValues);
    }
    std::uninitialized_copy(first, last, _end);
    _end += numValues;
  }

  void append(size_type numValues, const value_type & value) {
    if (numValues > size_type(_capacity - _end)) {
      this->grow(this->size() + numValues);
    }
    std::uninitialized_fill_n(_end, numValues, value);
    _end += numValues;
  }

  void assign(size_type numValues, const value_type & value) {
    clear();
    if (this->capacity() < numValues) {
      this->grow(numValues);
    }
    _end = _begin + numValues;
    construct_range(_begin, _end, value);
  }

  iterator erase(iterator i) {
    iterator result = i;
    std::copy(i + 1, _end, i);
    pop_back();
    return result;
  }

  iterator erase(iterator first, iterator last) {
    iterator result = first;
    iterator endPos = std::copy(last, _end, first);
    this->destroy_range(endPos, _end);
    _end = endPos;
    return result;
  }

  iterator insert(iterator insertPos, const value_type & value) {
    if (insertPos == _end) {  // Important special case for empty vector.
      push_back(value);
      return _end - 1;
    }

    if (_end >= _capacity) {
      size_t index = insertPos - _begin;
      this->grow();
      insertPos = _begin + index;
    }
    new (_end) T(this->back());
    _end++;
    std::copy_backward(insertPos, _end - 1, _end);

    // If we just moved the element we're inserting, be sure to update
    // the reference.
    const T * valuePtr = &value;
    if (insertPos <= valuePtr && valuePtr < _end) {
      ++valuePtr;
    }

    *insertPos = *valuePtr;
    return insertPos;
  }

  iterator insert(iterator insertPos, size_type count, const value_type & value) {
    if (insertPos == _end) {  // Important special case for empty vector.
      append(count, value);
      return _end - 1;
    }

    // Convert iterator to elt# to avoid invalidating iterator when we reserve()
    size_t offset = insertPos - _begin;

    // Ensure there is enough space.
    reserve(static_cast<unsigned>(this->size() + count));

    // Uninvalidate the iterator.
    insertPos = _begin + offset;

    // If there are more elements between the insertion point and the end of the
    // range than there are being inserted, we can use a simple approach to
    // insertion.  Since we already reserved space, we know that this won't
    // reallocate the vector.
    if (size_t(_end - insertPos) >= count) {
      T * oldEnd = _end;
      append(_end - count, _end);

      // Copy the existing elements that get replaced.
      std::copy_backward(insertPos, oldEnd - count, oldEnd);

      std::fill_n(insertPos, count, value);
      return insertPos;
    }

    // Otherwise, we're inserting more elements than exist already, and we're
    // not inserting at the end.

    // Copy over the elements that we're about to overwrite.
    T * oldEnd = _end;
    _end += count;
    size_t numOverwritten = oldEnd - insertPos;
    std::uninitialized_copy(insertPos, oldEnd, _end - numOverwritten);

    // Replace the overwritten part.
    std::fill_n(insertPos, numOverwritten, value);

    // Insert the non-overwritten middle part.
    std::uninitialized_fill_n(oldEnd, count - numOverwritten, value);
    return insertPos;
  }

  template<typename InputIterTy>
  iterator insert(iterator insertPos, InputIterTy first, InputIterTy last) {
    if (insertPos == _end) {  // Important special case for empty vector.
      append(first, last);
      return _end - 1;
    }

    size_t count = std::distance(first, last);
    // Convert iterator to elt# to avoid invalidating iterator when we reserve()
    size_t offset = insertPos - _begin;

    // Ensure there is enough space.
    reserve(static_cast<unsigned>(this->size() + count));

    // Uninvalidate the iterator.
    insertPos = _begin + offset;

    // If there are more elements between the insertion point and the end of the
    // range than there are being inserted, we can use a simple approach to
    // insertion.  Since we already reserved space, we know that this won't
    // reallocate the vector.
    if (size_t(_end - insertPos) >= count) {
      T *oldEnd = _end;
      append(_end - count, _end);

      // Copy the existing elements that get replaced.
      std::copy_backward(insertPos, oldEnd - count, oldEnd);

      std::copy(first, last, insertPos);
      return insertPos;
    }

    // Otherwise, we're inserting more elements than exist already, and we're
    // not inserting at the end.

    // Copy over the elements that we're about to overwrite.
    T *oldEnd = _end;
    _end += count;
    size_t numOverwritten = oldEnd - insertPos;
    std::uninitialized_copy(insertPos, oldEnd, _end - numOverwritten);

    // Replace the overwritten part.
    for (; numOverwritten > 0; --numOverwritten) {
      *insertPos = *first;
      ++insertPos; ++first;
    }

    // Insert the non-overwritten middle part.
    std::uninitialized_copy(first, last, oldEnd);
    return insertPos;
  }

  const SmallVectorImpl & operator=(const SmallVectorImpl & rhs);

  bool operator==(const SmallVectorImpl & rhs) const {
    if (this->size() != rhs.size()) {
      return false;
    }
    return std::equal(_begin, _end, rhs._begin);
  }

  bool operator!=(const SmallVectorImpl & rhs) const {
    return !(*this == rhs);
  }

  bool operator<(const SmallVectorImpl & rhs) const {
    return std::lexicographical_compare(_begin, _end, rhs._begin, rhs._end);
  }

private:
  /** Dummy class used in calculating the offset to the first array element. */
  class Sizer : public SmallVectorImpl {
  public:
    bool isSmallImpl() const {
      return _begin == &_elements[0];
    }

    pointer internalBufferImpl() { return &_elements[0]; }

    T _elements[1];
  };

  /// True if we're still using the built-in buffer
  bool isSmall() const {
    return static_cast<const Sizer *>(this)->isSmallImpl();
  }

  /// Address of the built-in buffer
  pointer internalBuffer() {
    return static_cast<Sizer *>(this)->internalBufferImpl();
  }

  /// grow - double the size of the allocated memory, guaranteeing space for at
  /// least one more element or minSize if specified.
  void grow(size_t minSize = 0);

  /// Run constructors for a range of elements.
  static void construct_range(T * first, T * last, const T & value) {
    for (; first != last; ++first) {
      new (first) T(value);
    }
  }

  /// Run destructors for a range of elements.
  static void destroy_range(pointer first, pointer last) {
    while (first != last) {
      --last;
      last->~T();
    }
  }

  // Declaring them this way instead of void* makes them easier to inspect in
  // the debugger.
  pointer _begin;
  pointer _end;
  pointer _capacity;
};

// Define this out-of-line to dissuade the C++ compiler from inlining it.
template <typename T>
void SmallVectorImpl<T>::grow(size_t minSize) {
  size_t oldCapacity = this->capacity();
  size_t oldSize = this->size();
  size_t newCapacity = 2 * oldCapacity + 1; // Always grow, even from zero.
  if (newCapacity < minSize) {
    newCapacity = minSize;
  }
  T * newData = static_cast<T*>(malloc(newCapacity*sizeof(T)));

  // Copy the elements over.
  std::uninitialized_copy(_begin, _end, newData);

  // Destroy the original elements.
  destroy_range(_begin, _end);

  // If this wasn't grown from the inline copy, deallocate the old space.
  if (!isSmall()) {
    free(_begin);
  }

  _end = newData + oldSize;
  _begin = newData;
  _capacity = _begin + newCapacity;
}

/** -------------------------------------------------------------------------
    A vector class that uses a fixed-length buffer, with overflow capability.
    (Loosely based on LLVM's SmallVector class, but this one is much simpler
    since we don't need as much flexibility.)
 */
template <typename T, unsigned N>
class SmallVector : public SmallVectorImpl<T> {
public:
  /// Default constructor
  SmallVector() : SmallVectorImpl<T>(N) {}

  explicit SmallVector(unsigned count, const T & value = T()) : SmallVectorImpl<T>(N) {
    this->reserve(count);
    while (count--) {
      this->push_back(value);
    }
  }

  template<typename InputIterTy>
  SmallVector(InputIterTy first, InputIterTy last) : SmallVectorImpl<T>(N) {
    this->append(first, last);
  }

  SmallVector(const SmallVector & rhs) : SmallVectorImpl<T>(N) {
    if (!rhs.empty()) {
      SmallVectorImpl<T>::operator=(rhs);
    }
  }

  const SmallVector &operator=(const SmallVector & rhs) {
    SmallVectorImpl<T>::operator=(rhs);
    return *this;
  }

private:
  T _elements[N];
};

/** -------------------------------------------------------------------------
    Specialization of SmallVector at N=0.
 */
template <typename T>
class SmallVector<T, 0> : public SmallVectorImpl<T> {
public:
  SmallVector() : SmallVectorImpl<T>(0) {}

  explicit SmallVector(unsigned count, const T & value = T()) : SmallVectorImpl<T>(0) {
    this->reserve(count);
    while (count--) {
      this->push_back(value);
    }
  }

  template<typename InputIterTy>
  SmallVector(InputIterTy first, InputIterTy last) : SmallVectorImpl<T>(0) {
    this->append(first, last);
  }

  SmallVector(const SmallVector & rhs) : SmallVectorImpl<T>(0) {
    if (!rhs.empty()) {
      SmallVectorImpl<T>::operator=(rhs);
    }
  }

  const SmallVector &operator=(const SmallVector & rhs) {
    SmallVectorImpl<T>::operator=(rhs);
    return *this;
  }
};

}

#endif // MINT_COLLECTIONS_SMALLVECTOR_H
