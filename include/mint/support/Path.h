/* ================================================================ *
   Path manipulation functions.
 * ================================================================ */

#ifndef MINT_SUPPORT_PATH_H
#define MINT_SUPPORT_PATH_H

#ifndef MINT_COLLECTIONS_STRINGREF_H
#include "mint/collections/StringRef.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLVECTOR_H
#include "mint/collections/SmallVector.h"
#endif

#ifndef MINT_COLLECTIONS_ARRAYREF_H
#include "mint/collections/ArrayRef.h"
#endif

#ifndef MINT_SUPPORT_TIMESTAMP_H
#include "mint/support/TimeStamp.h"
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

// 'native_char_t' is whatever type the native file API wants.
#if defined(_MSC_VER)
  typedef wchar_t native_char_t;
#else
  typedef char native_char_t;
#endif

namespace mint {
namespace path {

/// Return true if character 'ch' is a directory separator character.
inline bool isDirectorySeparator(char ch) {
  return ch == '/';
}

/// Find the first separator character in 'path' occurring at or after position 'pos'.
/// Returns -1 if no separator can be found.
int findSeparatorFwd(const char * path, size_t size, int pos = 0);

/// Find the first separator character in 'path' occurring at or after position 'pos'.
/// Returns -1 if no separator can be found.
inline int findSeparatorFwd(StringRef path, int pos = 0) {
  return findSeparatorFwd(path.data(), path.size(), pos);
}

/// Find the last separator character in 'path' occuring before position 'pos'.
/// Returns -1 if no separator can be found.
int findSeparatorRvs(const char * path, size_t size, int pos = INT_MAX);

/// Find the last separator character in 'path' occuring before position 'pos'.
/// Returns -1 if no separator can be found.
inline int findSeparatorRvs(StringRef path, int pos = INT_MAX) {
  return findSeparatorRvs(path.data(), path.size(), pos);
}

/// Find the end of the file or directory name start at position 'pos'.
/// Returns 'size' if no separator can be found.
unsigned findNameEnd(const char * path, size_t size, int pos = 0);

/// Find the end of the file or directory name start at position 'pos'.
/// Returns path.size if no separator can be found.
inline unsigned findNameEnd(StringRef path, int pos = 0) {
  return findNameEnd(path.data(), path.size(), pos);
}

/// Return the index of the file extension (not including the dot), or -1 if none can be found.
int findExtension(const char * path, size_t size);

/// Return the index of the file extension (not including the dot), or -1 if none can be found.
inline int findExtension(StringRef path) {
  return findExtension(path.data(), path.size());
}

/// Return true if the last character in the string is a directory separator.
bool hasTrailingSeparator(const char * path, size_t size);

/// Return true if the last character in the string is a directory separator.
inline bool hasTrailingSeparator(StringRef path) {
  return hasTrailingSeparator(path.data(), path.size());
}

/// If this path begins with an absolute path root such as '/', locate the end of the root.
/// Otherwise, return 0.
int findRoot(const char * path, size_t size);

/// If this path begins with an absolute path root such as '/', locate the end of the root.
/// Otherwise, return 0.
inline int findRoot(StringRef path);

// def makeAbsolute(path:String) -> String;

/// Convert path 'path' to canonical form, removing current directory ('.') and parent
/// directory ('..') path components where possible.
void normalize(SmallVectorImpl<char> & path);

// def isReadable(path:String) -> bool;
// def isWritable(path:String) -> bool;

bool isAbsolute(StringRef path);

/// Return the filename portion of the path - that is, everything past the last directory separator
/// character. If there's no separator, then return the entire input path unchanged.
/// If the last character in the path is a separator, then this will return a 0-length string.
StringRef filename(StringRef path);

/// Returns a string containing all characters in 'path' prior to the last path separator.
/// If there is no directory separator, then it returns the empty string. If the path consists only
/// of a single separator, then it returns the path unchanged.
StringRef parent(StringRef path);

/// Split the path on the final directory separator, returning the portion before and after the
/// separator as separate strings. If there is no separator, then it returns an empty string for
/// the first part, and the original input string for the second part.
std::pair<StringRef, StringRef> split(StringRef path);

/// Return the file extension of 'path' or the empty string if no file extension can be found.
StringRef extension(StringRef path);

/// Replace the extension of 'path' with the string 'ext'. 'ext' should not have a
/// leading '.' character.
void changeExtension(SmallVectorImpl<char> & path, StringRef ext);

/// Combine 'path' and 'newpath' by resolving 'newpath' relative to 'path'. If 'newpath'
/// is an absolute path, then 'path' will be set to 'newpath'. This function does not normalize
/// the result.
void concat(SmallVectorImpl<char> & path, StringRef newpath);

/// Combine 'path' and 'newpath' by resolving 'newpath' relative to 'path'. If 'newpath'
/// is an absolute path, then 'path' will be set to 'newpath' will be returned.
/// In all cases, the returned path will be normalized.
void combine(SmallVectorImpl<char> & path, StringRef newpath);

/// Transform 'path' so that it is relative to 'base', and store the result in 'result'.
/// Both 'path' and 'base' must be absolute paths. If 'path' cannot be made relative to 'basePath'
/// (perhaps because it's in a different namespace or has a different drive specification), then
/// 'relpath' is set to a copy of 'path' 'false' is returned. Otherwise returns true.
bool makeRelative(StringRef base, StringRef path, SmallVectorImpl<char> & result);

/// Convert the path 'in' from UTF-8 to native character format. This also
/// ensures that the output path is null-terminated.
void toNative(StringRef in, SmallVectorImpl<native_char_t> & out);

/// Convert from a native path back to UTF-8.
void fromNative(ArrayRef<native_char_t> in, SmallVectorImpl<char> & out);

/// Get the current working directory
void getCurrentDir(SmallVectorImpl<char> & path);

enum Requirements {
  IS_FILE = (1<<0),             // Require that the entry is a regular file
  IS_DIRECTORY = (1<<1),        // Require that the entry is a directory
  IS_READABLE = (1<<2),         // Require that the entry be readable
  IS_WRITABLE = (1<<3),         // Require that the entry be writable
  IS_SEARCHABLE = (1<<4),       // Require that the entry be searchable
};

/// Test that 'path' meets all the requirements specified in the flags. Returns
/// false if the path does not meet the conditions.
bool test(StringRef path, unsigned requirements, bool quiet = false);

struct FileStatus {
  bool exists;
  bool isFile;
  bool isDir;
  //bool readable;
  //bool writeable;
  size_t size;
  TimeStamp lastModified;

  FileStatus() : exists(false), isFile(false), isDir(false), size(0) {}
};

/// Get the file status of this file. It's OK if the file does not exist or lacks permissions,
/// but other kinds of errors will cause a fatal error message. Returns the result in the
/// provided FileStatus structure.
bool fileStatus(StringRef path, FileStatus & status);

/// Read the contents of a file located at 'path' into 'buffer'.
/// Return false if there was an error.
bool readFileContents(StringRef path, SmallVectorImpl<char> & buffer);

/// Write the contents of a file located at 'path' from 'content'. Automatically
/// creates parent directories if needed. Return false if there was an error.
bool writeFileContents(StringRef path, StringRef content);

/// Copy the contents of the file at 'sourcePath' to the file at 'outputPath'.
bool copyFile(StringRef sourcePath, StringRef outputPath);

/// Read the contents of a file located at 'path' into 'buffer', and check if it is
/// different from the text in 'newContent'. If it is, then overwrite the contents
/// of the file with 'newContent'.
bool writeFileContentsIfDifferent(StringRef path, StringRef newContent);

/// Checks that each directory in 'path' exists, and if not, creates it.
bool makeDirectoryPath(StringRef path);

/// Delete a file from the file system.
bool remove(StringRef path);

}
}

#endif // MINT_SUPPORT_PATH_H
