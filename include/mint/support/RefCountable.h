/* ================================================================ *
   Reference counting.
 * ================================================================ */

#ifndef MINT_SUPPORT_REFCOUNTABLE_H
#define MINT_SUPPORT_REFCOUNTABLE_H

#ifndef MINT_CONFIG_H
#include "mint/config.h"
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Base class for reference-countable objects.
 */
class RefCountable {
public:

  /// Constructor
  RefCountable() : _refCount(0) {}

  /// Destructor must be virtual.
  virtual ~RefCountable() {}

  /// Increment the reference count by one.
  void acquire() const { ++_refCount; }

  /// Decrement the reference count by one, and release the object if it
  /// falls to zero.
  void release() const { if (--_refCount == 0) delete this; }

  /// Static helper function that increments a reference countable object.
  /// Handles NULL pointers.
  static inline void acquire(const RefCountable * ptr) {
    if (ptr != NULL) { ptr->_refCount++; }
  }

  /// Static helper function that decrements a reference countable object.
  /// Handles NULL pointers.
  static inline void release(const RefCountable * ptr) {
    if (ptr != NULL && --ptr->_refCount == 0) delete ptr;
  }

  /// For purposes of debugging and tracing it can be useful to expose
  /// the current reference count.
  unsigned refCount() const { return _refCount; }

private:
  mutable unsigned _refCount;
};

/** -------------------------------------------------------------------------
    Smart-pointer class for reference countable objects.
 */
template<class T>
class Ref {
public:
  /// Default constructor initializes the pointer to zero.
  Ref() : _ptr(NULL) {}

  /// Copy constructor
  Ref(const Ref & src) : _ptr(src._ptr) { RefCountable::acquire(_ptr); }

  /// Copy constructor
  Ref(T * ptr) : _ptr(ptr) { RefCountable::acquire(ptr); }

  /// Destructor
  ~Ref() { RefCountable::release(_ptr); }

  /// Assignment operator
  Ref & operator=(const Ref & src) {
    T * oldPtr = _ptr;
    RefCountable::acquire(src._ptr);
    _ptr = src._ptr;
    RefCountable::release(oldPtr);
  }

  /// Assignment operator
  Ref & operator=(T * newPtr) {
    T * oldPtr = _ptr;
    RefCountable::acquire(newPtr);
    _ptr = newPtr;
    RefCountable::release(oldPtr);
    return *this;
  }

  /// Accessor method for the raw pointer.
  T * ptr() const { return _ptr; }

  /// Pointer dereference
  const T * operator->() const { return _ptr; }
  T * operator->() { return _ptr; }

  /// Comparators
  bool operator==(const T * newPtr) {
    return _ptr == newPtr;
  }

  /// Comparators
  bool operator!=(const T * newPtr) {
    return _ptr != newPtr;
  }

private:
  T * _ptr;
};

}

#endif // MINT_SUPPORT_REFCOUNTABLE_H
