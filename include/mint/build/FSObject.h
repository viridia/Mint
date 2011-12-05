/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_BUILD_FSOBJECT_H
#define MINT_BUILD_FSOBJECT_H

#ifndef MINT_SUPPORT_GC_H
#include "mint/support/GC.h"
#endif

#ifndef MINT_GRAPH_STRINGDICT_H
#include "mint/graph/StringDict.h"
#endif

namespace mint {

class File;
class Directory;

/** -------------------------------------------------------------------------
    A file system object, such as a file or directory.
 */
class FSObject : public GC {
public:

  /// Constructor
  FSObject(Directory * parent, String * name)
    : _parent(parent)
    , _name(name)
  {}

  /// Destructor
  virtual ~FSObject() {}

  /// The directory containing this object.
  Directory * parent() const { return _parent; }

  /// Name of this object.
  String * name() const { return _name; }

  /// Garbage collection trace function.
  void trace() const;

private:
  // last modified date.
  Directory * _parent;
  String * _name;
};

}

#endif // MINT_BUILD_FSOBJECT_H
