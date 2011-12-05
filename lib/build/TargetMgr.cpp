/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/build/TargetMgr.h"

namespace mint {

Target * TargetMgr::getTarget(Object * targetDefinition) {
  TargetMap::const_iterator it = _targets.find(targetDefinition);
  if (it != _targets.end()) {
    return it->second;
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
  File * file = new File(NULL, filePath);
  _files[filePath] = file;
  return file;
}

void TargetMgr::trace() const {
  _targets.trace();
  _files.trace();
}

}
