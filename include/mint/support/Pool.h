/* ================================================================ *
   Memory Pool.
 * ================================================================ */

#ifndef MINT_SUPPORT_POOL_H
#define MINT_SUPPORT_POOL_H

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLVECTOR_H
#include "mint/collections/SmallVector.h"
#endif

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#include "mint/collections/ArrayRef.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Memory allocation pool, items are allocated individually and then
    freed all at once.
 */
class Pool {
public:
  enum {
    DEFAULT_BLOCK_SIZE = 64 * 1024,  // 64k blocks
    LARGE_ALLOC_SIZE = 16 * 1024,    // Items > 16k get their own block
    ALIGNMENT = 16,                  // Alignment of all allocations
  };

  /// Constructor
  Pool();

  /// Destructor
  ~Pool() { clear(); }

  /// Allocate a block of memory from the pool
  void * alloc(size_t size);

  /// Make a copy of the input string.
  StringRef makeString(StringRef str);
  StringRef makeString(SmallVectorImpl<char> str);

  /// Make a copy of an array. Data is copied as POD.
  template<class T> T * makeArrayCopy(const T * data, unsigned size) {
    if (size == 0) {
      return NULL;
    }
    size *= sizeof(T);
    char * mem = static_cast<char *>(alloc(size));
    memcpy(mem, data, size);
    return reinterpret_cast<T*>(mem);
  }

  /// Make a copy of an array. Data is copied as POD.
  template<class T> ArrayRef<T> makeArrayCopy(ArrayRef<T> array) {
    return ArrayRef<T>(makeArrayCopy(array.data(), array.size()), array.size());
  }

  /// Free the entire pool
  void clear();

  /// Align n to nearest ALIGNMENT boundary
  static size_t align(size_t n) {
    return (n + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
  }

  /// Return the total amount of memory allocated
  size_t size();

  /// Return the current available space
  size_t avail();

  /// Return how much space is unused
  size_t unused();

private:
  struct Block {
    Block * next;
    char * pos;
    char * end;
  };

  /// Allocate a new block containing 'size' free space.
  Block * allocBlock(size_t size);

  /// Pointer to most recently allocated block.
  Block * _front;
};

}

#endif // MINT_SUPPORT_POOL_H
