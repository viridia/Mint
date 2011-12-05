/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_BUILD_TARGETMGR_H
#define MINT_BUILD_TARGETMGR_H

#ifndef MINT_BUILD_TARGET_H
#include "mint/build/Target.h"
#endif

#ifndef MINT_BUILD_FILE_H
#include "mint/build/File.h"
#endif

#ifndef MINT_GRAPH_OBJECT_H
#include "mint/graph/Object.h"
#endif

#if HAVE_STDDEF_H
#include <stddef.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    TableKeyTraits for Strings.
 */
struct ObjectPointerKeyTraits {
  static inline unsigned hash(const Object * key) {
    return (unsigned(uintptr_t(key)) >> 4) ^ (unsigned(uintptr_t(key)) >> 9);
  }

  static inline unsigned equals(const Object * l, const Object * r) {
    return l == r;
  }
};

/** -------------------------------------------------------------------------
    Dictionary type that maps from object pointers to targets.
 */
class TargetMap : public Table<Object, Target, ObjectPointerKeyTraits> {
public:
  TargetMap(size_t initialSize = 0) : Table<Object, Target, ObjectPointerKeyTraits>(initialSize) {}
  void trace() const {
    for (TargetMap::const_iterator it = this->begin(), itEnd = this->end(); it != itEnd;
        ++it) {
      it->first->mark();
      it->second->mark();
    }
  }
};

/** -------------------------------------------------------------------------
    A build target.
 */
class TargetMgr : public GC {
public:
  typedef StringDict<File> FileMap;

  /// Constructor
  TargetMgr() {}

  /// Map of all named targets.
  const TargetMap & targets() const { return _targets; }

  /// Given an object representing a target, return the target, creating it if needed.
  Target * getTarget(Object * targetDefinition);

  /// Given an absolute path to a file, return the File object, creating it if needed.
  File * getFile(String * filePath);

  /// Garbage collection trace function.
  void trace() const;

private:
  TargetMap _targets;
  FileMap _files;
};

}

#endif // MINT_BUILD_TARGETMGR_H
