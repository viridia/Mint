/* ================================================================ *
   Functions for dealing with directories.
 * ================================================================ */

#ifndef MINT_SUPPORT_DIRECTORY_H
#define MINT_SUPPORT_DIRECTORY_H

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLVECTOR_H
#include "mint/collections/SmallVector.h"
#endif

#if HAVE_DIRENT_H
#include <dirent.h>
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Class to iterate through all of the file system entries in a directory.
 */
class DirectoryIterator {
public:
  /// Constructor
  DirectoryIterator();

  /// Destructor
  ~DirectoryIterator() {
    finish();
  }

  /// Begin a new iteration, for the directory located at 'dirPath'.
  /// Returns false if there was an error (and prints appropriate
  /// error messages to stderr.)
  /// You should call 'next' after calling this method to get the
  /// first filesystem entry.
  bool begin(StringRef dirPath);

  /// The name of the current directory entry. This will only be valid
  /// until the next call to 'next'.
  const char * entryName() const;

  /// Returns true if the current directory entry is a directory.
  /// This will only be valid until the next call to 'next'.
  bool isDirectory() const;

  /// Advance to the next entry. Returns false if there is no next entry.
  bool next();

  /// Release any resources held by this object.
  void finish();

private:
  #if HAVE_DIRENT_H
    DIR * _dirp;
    const char * _entryName;
    bool _isDirectory;
  #endif
};

}

#endif // MINT_SUPPORT_DIRECTORY_H
