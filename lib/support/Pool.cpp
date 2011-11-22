/* ================================================================ *
   Memory Pool.
 * ================================================================ */

#include "mint/support/Pool.h"

#if HAVE_STRING_H
#include <string.h>
#endif

namespace mint {

Pool::Pool() : _front(NULL) {
}

/// Allocate a block of memory from the pool
void * Pool::alloc(size_t size) {
  size = align(size);
  if (size < LARGE_ALLOC_SIZE) {
    /// If there's not enough room, create a new block.
    if (_front == NULL || _front->pos + size > _front->end) {
      Block * b = allocBlock(DEFAULT_BLOCK_SIZE);
      b->next = _front;
      _front = b;
    }

    char * result = _front->pos;
    _front->pos += size;
    return result;
  } else {
    // Large objects get their own block.
    Block * b = allocBlock(size);
    // Don't put it at the head of the block list.
    if (_front) {
      b->next = _front->next;
      _front->next = b;
    } else {
      b->next = NULL;
      _front = b;
    }
    char * result = b->pos;
    b->pos += size;
    return result;
  }
}

/// Make a copy of the input string.
StringRef Pool::makeString(StringRef str) {
  size_t size = str.size();
  char * mem = static_cast<char *>(alloc(size));
  memcpy(mem, str.data(), size);
  return StringRef(mem, size);
}

StringRef Pool::makeString(SmallVectorImpl<char> str) {
  size_t size = str.size();
  char * mem = static_cast<char *>(alloc(size));
  memcpy(mem, str.data(), size);
  return StringRef(mem, size);
}

/// Free the entire pool
void Pool::clear() {
  while (_front) {
    Block * b = _front;
    _front = b->next;
    delete b;
  }
}

Pool::Block * Pool::allocBlock(size_t size) {
  size_t headerSize = align(sizeof(Block));
  size_t totalSize = align(headerSize + size);
  char * mem = new char[totalSize]();
  Block * b = reinterpret_cast<Block *>(mem);
  b->pos = mem + headerSize;
  b->end = mem + totalSize;
  return b;
}

size_t Pool::size() {
  size_t result = 0;
  for (Block * b = _front; b != NULL; b = b->next) {
    char * begin = reinterpret_cast<char *>(b) + align(sizeof(Block));
    result += b->pos - begin;
  }
  return result;
}

size_t Pool::avail() {
  return _front ? _front->end - _front->pos : 0;
}

size_t Pool::unused() {
  size_t result = 0;
  for (Block * b = _front; b != NULL; b = b->next) {
    result += b->end - b->pos;
  }
  return result;
}

}
