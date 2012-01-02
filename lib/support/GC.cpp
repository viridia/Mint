/* ================================================================== *
 * Garbage collection
 * ================================================================== */

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/GC.h"

#if HAVE_MALLOC_H
#include <malloc.h>
#elif HAVE_MALLOC_MALLOC_H
#include <malloc/malloc.h>
#endif

namespace mint {

#define GC_DEBUG 1

// -------------------------------------------------------------------------
// GC
// -------------------------------------------------------------------------

bool GC::_initialized = false;
unsigned GC::_debugLevel = 0;
GC * GC::_allocList = NULL;
GCRootBase * GC::_roots = NULL;

unsigned char GC::_cycleIndex = 0;

void * GC::operator new(size_t, size_t actualSize) {
  return alloc(actualSize);
}

void GC::operator delete(void * mem) {}
void GC::operator delete(void * mem, size_t actualSize) {}

void GC::init() {
  M_ASSERT(!_initialized) << "Garbage collector has already been initialized!";
  _initialized = true;
}

void GC::uninit() {
  M_ASSERT(_initialized) << "Garbage collector has not been initialized!";
  _initialized = false;
}

void * GC::alloc(size_t size) {
  M_ASSERT(_initialized) << "Garbage collector has not been initialized!";
  GC * gc = reinterpret_cast<GC *>(malloc(size));
  #if GC_DEBUG
    memset((void *)gc, 0xDB, size);
  #endif
  gc->_next = _allocList;
  gc->_cycle = _cycleIndex;
  _allocList = gc;
  return gc;
}

void GC::sweep() {
  unsigned reclaimed = 0;
  unsigned total = 0;

  // Increment the collection cycle index
  ++_cycleIndex;

  // Trace all roots.
  for (GCRootBase * root = _roots; root != NULL; root = root->_next) {
    root->trace();
  }

  // Delete any allocated object not marked.
  GC ** ptr = &_allocList;
  while (GC * gc = *ptr) {
    if (gc->_cycle == _cycleIndex) {
      ptr = &gc->_next;
    } else {
      *ptr = gc->_next;
      gc->~GC();
      #if GC_DEBUG
        #if HAVE_MALLOC_SIZE
          size_t sz = malloc_size(gc);
          memset((void *)gc, 0xDF, sz);
        #elif HAVE_MALLOC_USABLE_SIZE
          size_t sz = malloc_usable_size(gc);
          memset((void *)gc, 0xDF, sz);
        #endif
      #endif
      free(gc);
      ++reclaimed;
    }
    ++total;
  }

  if (_debugLevel) {
    diag::info(Location()) << "GC: " << reclaimed <<
        " objects reclaimed, " << (total - reclaimed) << " in use";
  }
}

void GC::setDebugLevel(unsigned level) {
  _debugLevel = level;
}

// -------------------------------------------------------------------------
// GCRootBase
// -------------------------------------------------------------------------

GCRootBase::GCRootBase() {
  _next = GC::_roots;
  GC::_roots = this;
}

}
