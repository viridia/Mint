/* ================================================================== *
 * Mint
 * ================================================================== */

#ifndef MINT_BUILD_DIRECTORY_H
#define MINT_BUILD_DIRECTORY_H

#ifndef MINT_BUILD_FILE_H
#include "mint/build/File.h"
#endif

#ifndef MINT_GRAPH_STRINGDICT_H
#include "mint/graph/StringDict.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    A directory on the filesystem.
 */
class Directory : public GC {
public:
  typedef StringDict<File> Files;
  typedef StringDict<Directory> Directories;

  /// Constructor
  Directory(Directory * parent, String * name)
    : _parent(parent)
    , _name(name)
    , _statusChecked(false)
    , _statusValid(false)
  {}

  /// Destructor
  virtual ~Directory() {}

  /// The parent directory of this one.
  Directory * parent() const { return _parent; }

  /// Name of this directory.
  String * name() const { return _name; }

  /// True if the file status is up to date.
  bool statusChecked() const { return _statusChecked; }

  /// True if the file status is valid, that we didn't get an error when testing it.
  bool statusValid() const { return _statusValid; }

  /// Query the file system and update the status of this directory.
  bool updateDirectoryStatus();

  /// Return true if this file exists.
  bool exists() const { return _status.exists; }

  /// Create this directory.
  bool create();

  /// Return the last-modified time of the file.
  const TimeStamp & lastModified() const { return _status.lastModified; }

  /// Files in this directory
  const Files & files() const { return _files; }
  void add(File * file) { _files[file->name()] = file; }

  /// Contents of this directory
  const Directories & subdirs() const { return _subdirs; }
  void add(Directory * dir) { _subdirs[dir->name()] = dir; }

  /// Garbage collection trace function.
  void trace() const;

private:
  Directory * _parent;
  String * _name;
  Files _files;
  Directories _subdirs;
  path::FileStatus _status;
  bool _statusChecked;
  bool _statusValid;
};

}

#endif // MINT_BUILD_TARGET_H
