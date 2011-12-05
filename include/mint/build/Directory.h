/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_BUILD_DIRECTORY_H
#define MINT_BUILD_DIRECTORY_H

#ifndef MINT_BUILD_FSOBJECT_H
#include "mint/build/FSObject.h"
#endif

#ifndef MINT_GRAPH_STRINGDICT_H
#include "mint/graph/StringDict.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    A directory on the filesystem.
 */
class Directory : public FSObject {
public:
  typedef StringDict<FSObject> Contents;

  /// Constructor
  Directory(Directory * parent, String * name)
    : FSObject(parent, name)
  {}

  /// Destructor
  virtual ~Directory() {}

  /// Contents of this directory
  const Contents & contents() const { return _contents; }

  /// Garbage collection trace function.
  void trace() const;

private:
  Contents _contents;
};

}

#endif // MINT_BUILD_TARGET_H
