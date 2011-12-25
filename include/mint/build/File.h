/* ================================================================== *
 * Node
 * ================================================================== */

#ifndef MINT_BUILD_FILE_H
#define MINT_BUILD_FILE_H

#ifndef MINT_SUPPORT_GC_H
#include "mint/support/GC.h"
#endif

#ifndef MINT_SUPPORT_OSTREAM_H
#include "mint/support/OStream.h"
#endif

#ifndef MINT_SUPPORT_PATH_H
#include "mint/support/Path.h"
#endif

namespace mint {

class Target;
class Directory;
class String;

typedef SmallVector<Target *, 8> TargetList;

/** -------------------------------------------------------------------------
    Represents a file on the file system, which may be an input or an output
    to a build target.
 */
class File : public GC {
public:

  /// Constructor
  File(Directory * parent, String * name)
    : _parent(parent)
    , _name(name)
    , _statusChecked(false)
    , _statusValid(false)
  {}

  /// Destructor
  virtual ~File() {}

  /// The directory containing this object.
  Directory * parent() const { return _parent; }

  /// Absolute path of this file.
  String * name() const { return _name; }

  /// True if the file status is up to date.
  bool statusChecked() const { return _statusChecked; }

  /// True if the file status is valid, that we didn't get an error when testing it.
  bool statusValid() const { return _statusValid; }

  /// Query the file system and update the status of this file.
  bool updateFileStatus();

  /// Return true if this file exists.
  bool exists() const { return _status.exists; }

  /// Return the last-modified time of the file.
  const TimeStamp & lastModified() const { return _status.lastModified; }

  /// Return the size of the file.
  size_t size() const { return _status.size; }

  /// Delete this file, if it exists.
  bool remove();

  /// Return the list of targets that need this file to be up to date
  const TargetList & sourceFor() const { return _sourceFor; }
  void addSourceFor(Target * target) {
    _sourceFor.push_back(target);
  }

  /// Return the list of targets that produce this file
  const TargetList & outputOf() const { return _outputOf; }
  void addOutputOf(Target * target) {
    _outputOf.push_back(target);
  }

  /// Garbage collection trace function.
  void trace() const;

private:
  Directory * _parent;
  String * _name;
  TargetList _sourceFor;
  TargetList _outputOf;
  path::FileStatus _status;
  bool _statusChecked;
  bool _statusValid;
};

/// Stream operator for Files.
OStream & operator<<(OStream & strm, const File * file);

}

#endif // MINT_BUILD_TARGET_H
