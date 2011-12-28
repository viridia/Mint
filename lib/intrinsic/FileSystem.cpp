/* ================================================================== *
 * Fundamentals
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Function.h"
#include "mint/graph/Module.h"
#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/DirectoryIterator.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"
#include "mint/support/Wildcard.h"

namespace mint {

void glob(
    Location loc, SmallVectorImpl<Node *> & dirOut, StringRef basePath, StringRef pattern) {
  int dirSep = path::findSeparatorFwd(pattern, 0);
  int nextPath = dirSep + 1;
  if (dirSep < 0) {
    dirSep = nextPath = pattern.size();
  }

  StringRef leadingDirPart = pattern.substr(0, dirSep);
  StringRef trailingDirPart = pattern.substr(nextPath);
  if (leadingDirPart == ".") {
    // Current directory indicator - just ignore it.
    glob(loc, dirOut, basePath, trailingDirPart);
  } else if (leadingDirPart == "..") {
    // Parent directory - not allowed in pattern.
    diag::error(loc) << "Parent directory '..' not allowed as argument to 'glob'.";
  } else if (leadingDirPart == "**") {
    if (!trailingDirPart.empty()) {
      if (trailingDirPart.startsWith("**")) {
        diag::error(loc) << "Multiple '**' wildcards are not allowed as argument to 'glob'.";
        return;
      }
      DirectoryIterator di;
      di.begin(basePath);
      SmallString<64> newPath;
      WildcardMatcher matcher(trailingDirPart);
      bool moreDirSeps = path::findSeparatorFwd(trailingDirPart, 0) >= 0;
      while (di.next()) {
        StringRef name = di.entryName();
        if (name == "." || name == "..") {
          continue;
        }
        // Combine base path with fs entry.
        newPath.assign(basePath);
        path::combine(newPath, name);
        if (di.isDirectory()) {
          // For directories, we try twice
          glob(loc, dirOut, newPath, trailingDirPart);
          glob(loc, dirOut, newPath, pattern);
        } else if (!moreDirSeps) {
          if (matcher.match(name)) {
            dirOut.push_back(String::create(loc, newPath));
          }
        }
      }
      di.finish();
    }
  } else if (WildcardMatcher::hasWildcardChars(leadingDirPart)) {
    //console::out() << "WC: " << basePath << "/{" << leadingDirPart << "}/" << trailingDirPart << "\n";
    // Make sure base path is even a directory
    if (path::test(basePath, path::IS_DIRECTORY | path::IS_SEARCHABLE, true)) {
      WildcardMatcher matcher(leadingDirPart);
      DirectoryIterator di;
      di.begin(basePath);
      SmallString<64> newPath;
      while (di.next()) {
        StringRef name = di.entryName();
        if (name == "." || name == "..") {
          continue;
        }
        if (matcher.match(name)) {
          // Combine base path with fs entry.
          newPath.assign(basePath);
          path::combine(newPath, name);
          if (di.isDirectory()) {
            // If it's a dir, only add if there are more pattern parts.
            if (!trailingDirPart.empty()) {
              glob(loc, dirOut, newPath, trailingDirPart);
            }
          } else {
            // If it's a file, only add if there are no more pattern parts.
            if (trailingDirPart.empty()) {
              dirOut.push_back(String::create(loc, newPath));
            }
          }
        }
      }
      di.finish();
    }
  } else {
    // leadingDirPart contains no wildcards, it's a constant.
    SmallString<64> newPath(basePath);
    path::combine(newPath, leadingDirPart);
    if (trailingDirPart.size() == 0) {
      // No more pattern parts - if this isn't a file, then there's no match
      if (path::test(newPath, path::IS_FILE, true)) {
        dirOut.push_back(String::create(loc, newPath));
      }
    } else {
      // More pattern parts, which means that there's more searching to do.
      glob(loc, dirOut, newPath, trailingDirPart);
    }
  }
}

Node * methodGlob(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  String * pathArg = String::cast(args[0]);

  SmallVector<Node *, 64> dirs;
  if (path::isAbsolute(pathArg->value())) {
    // Absolute paths not allowed
    diag::error(pathArg->location()) << "Absolute path not allowed as argument to 'glob'.";
  } else {
    Module * m = ex->lexicalScope()->module();
    M_ASSERT(m != NULL);
    glob(pathArg->location(), dirs, m->sourceDir(), pathArg->value());
    if (dirs.empty()) {
      diag::warn(loc) << "No files found matching pattern.";
    }
  }

  return Oper::createList(pathArg->location(), fn->returnType(), dirs);
}

void initDirSearchMethods(Fundamentals * fundamentals) {
  // Function 'glob'.
  Type * typeStringList = TypeRegistry::get().getListType(TypeRegistry::stringType());
  fundamentals->defineMethod("glob", typeStringList, TypeRegistry::stringType(), methodGlob);
}

}
