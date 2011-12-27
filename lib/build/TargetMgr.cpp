/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/TargetMgr.h"

#include "mint/support/Diagnostics.h"

namespace mint {

Target * TargetMgr::getTarget(Object * targetDefinition, bool create) {
  TargetMap::const_iterator it = _targets.find(targetDefinition);
  if (it != _targets.end()) {
    return it->second;
  }
  if (!create) {
    return NULL;
  }
  Target * target = new Target(targetDefinition);
  _targets[targetDefinition] = target;
  return target;
}

File * TargetMgr::getFile(String * filePath) {
  FileMap::const_iterator it = _files.find(filePath);
  if (it != _files.end()) {
    return it->second;
  }
  Directory * parent = getParentDir(filePath->value());
  File * file = new File(parent, filePath);
  if (parent != NULL) {
    parent->add(file);
  }
  _files[filePath] = file;
  return file;
}

Directory * TargetMgr::getDirectory(String * dirPath) {
  DirectoryMap::const_iterator it = _dirs.find(dirPath);
  if (it != _dirs.end()) {
    return it->second;
  }
  Directory * parent = getParentDir(dirPath->value());
  Directory * dir = new Directory(parent, dirPath);
  if (parent != NULL) {
    parent->add(dir);
  }
  _dirs[dirPath] = dir;
  return dir;
}

Directory * TargetMgr::getDirectory(StringRef dirPath) {
  DirectoryMap::const_iterator it = _dirs.find_as(dirPath);
  if (it != _dirs.end()) {
    return it->second;
  }
  Directory * parent = getParentDir(dirPath);
  String * dirPathStr = String::create(dirPath);
  Directory * dir = new Directory(parent, dirPathStr);
  if (parent != NULL) {
    parent->add(dir);
  }
  _dirs[dirPathStr] = dir;
  return dir;
}

Directory * TargetMgr::getParentDir(StringRef path) {
  StringRef parentPath = mint::path::parent(path);
  if (parentPath.empty() || parentPath == path) {
    return NULL;
  }
  Directory * parentOfParent = getDirectory(parentPath);
  if (parentOfParent == NULL) {
    diag::warn() << "Parent directory not found: " << parentPath;
    return NULL;
  }
  return parentOfParent;
}

Directory * TargetMgr::addRootDirectory(StringRef dirPath) {
  DirectoryMap::const_iterator it = _dirs.find_as(dirPath);
  if (it != _dirs.end()) {
    return it->second;
  }
  String * dirPathStr = String::create(dirPath);
  Directory * dir = new Directory(NULL, dirPathStr);
  _dirs[dirPathStr] = dir;
  return dir;
}

Directory * TargetMgr::setBuildRoot(StringRef buildRoot) {
  if (_buildRoot != NULL) {
    diag::error() << "Build root already set!";
  } else {
    _buildRoot = addRootDirectory(buildRoot);
  }
  return _buildRoot;
}

void TargetMgr::deleteOutputFiles() {
  for (TargetMap::const_iterator it = _targets.begin(), itEnd = _targets.end(); it != itEnd; ++it) {
    Target * tg = it->second;
    if (!tg->isSourceOnly() && tg->state() != Target::CLEANING) {
      tg->setState(Target::CLEANING);
      for (FileList::const_iterator
          fi = tg->outputs().begin(), fiEnd = tg->outputs().end(); fi != fiEnd; ++fi) {
        File * file = *fi;
        // TODO: Don't remove files outside of the build directory.
        file->remove();
      }
      tg->setState(Target::CLEANED);
    }
  }
}

void TargetMgr::trace() const {
  _targets.trace();
  _files.trace();
  safeMark(_buildRoot);
}

}
