/* ================================================================ *
   Garbage collection.
 * ================================================================ */

#ifndef MINT_SUPPORT_GC_H
#define MINT_SUPPORT_GC_H

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#include "mint/collections/ArrayRef.h"
#endif

namespace mint {

class GCRootBase;

/** -------------------------------------------------------------------------
    Base class of garbage-collectible objects.
 */
class GC {
public:
  void * operator new(size_t size) { return alloc(size); }
  void * operator new(size_t, size_t actualSize);
  void operator delete(void * mem);

  /// Construct a new GC object.
  GC() : _cycle(0) {}

  /// Mark an object as in-use.
  void mark() const {
    if (_cycle != _cycleIndex) {
      _cycle = _cycleIndex;
      trace();
    }
  }

  /// Trace all references in this object.
  virtual void trace() const = 0;

  /// Initialize the GC heap.
  static void init();

  /// Tear down the GC heap.
  static void uninit();

  /// Delete all unmarked objects.
  static void sweep();

  /// Set the verbosity level.
  static void setDebugLevel(unsigned level);

  /// A version of mark which handles null pointers.
  template <class T>
  static void safeMark(T const * const ptr) {
    if (ptr != NULL) {
      ptr->mark();
    }
  }

  /** A version of mark which calls mark() all entries in an array. Should be used in cases
      where the array contents are GC pointers. */
  template <class T>
  static void markArray(ArrayRef<T> array) {
    for (typename ArrayRef<T>::const_iterator it = array.begin(); it != array.end(); ++it) {
      (*it)->mark();
    }
  }

  /** A version of mark which calls trace() on all entries in an array. Use in cases where
      the array contains structs which contains traceable pointers. */
  template <class T>
  static void traceArray(ArrayRef<T> array) {
    for (typename ArrayRef<T>::const_iterator it = array.begin(), itEnd = array.end();
        it != itEnd; ++it) {
      it->trace();
    }
  }

private:
  friend class GCRootBase;

  static void * alloc(size_t size);

  mutable unsigned char _cycle;
  GC * _next;

  static bool _initialized;
  static unsigned _debugLevel;
  static unsigned char _cycleIndex;
  static GC * _allocList;
  static GCRootBase * _roots;
};

/** -------------------------------------------------------------------------
    Class representing a garbage-collection root.
 */
class GCRootBase {
public:
  GCRootBase();
  virtual ~GCRootBase() {}

  /// Trace this root.
  virtual void trace() const = 0;

private:
  friend class GC;
  GCRootBase * _next;
};

/** -------------------------------------------------------------------------
    Class representing a garbage-collection root that holds a pointer.
 */
template<typename T>
class GCPointerRoot : public GCRootBase {
public:
  GCPointerRoot(T * ptr) : _ptr(ptr) {}
  virtual void trace() const { GC::safeMark(_ptr); }

  operator T *() { return _ptr; }
private:
  T * _ptr;
};

}

#endif

