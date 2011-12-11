/* ================================================================== *
 * ModuleLoader
 * ================================================================== */

#include "mint/parse/Parser.h"

#include "mint/eval/Evaluator.h"

#include "mint/graph/Object.h"

#include "mint/intrinsic/Fundamentals.h"

#include "mint/project/ModuleLoader.h"
#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"
#include "mint/support/TextBuffer.h"

namespace mint {

Module * ModuleLoader::load(StringRef mpath) {
  ModuleTable::const_iterator it = _modules.find_as(mpath);
  if (it != _modules.end()) {
    return it->second;
  }

  SmallString<128> relativePath;
  relativePath.resize(mpath.size());
  SmallString<128>::iterator out = relativePath.begin();
  for (StringRef::const_iterator it = mpath.begin(), itEnd = mpath.end(); it != itEnd; ++it) {
    if (*it == '.') {
      *out++ = '/';
    } else {
      *out++ = *it;
    }
  }

  SmallString<128> absPath(_sourceRoot);
  if (!relativePath.empty()) {
    path::combine(absPath, relativePath);
  }

  bool isDir = false;
  if (path::test(absPath, path::IS_DIRECTORY, true)) {
    path::combine(absPath, "module.mint");
    isDir = true;
  } else {
    path::changeExtension(absPath, "mint");
  }

  if (!path::test(absPath, path::IS_FILE | path::IS_READABLE, true)) {
    return NULL;
  }

  Module * m = new Module(mpath, _project);
  M_ASSERT(_project);
  m->setParentScope(&Fundamentals::get());
  if (_prelude != NULL) {
    m->addImportScope(_prelude);

    // If the prelude has been loaded, then we should know the source and build dirs too.
    SmallString<128> sourceDir(_sourceRoot);
    SmallString<128> buildDir(_project->buildRoot());
    if (isDir) {
      path::combine(sourceDir, relativePath);
      path::combine(buildDir, relativePath);
    } else {
      path::combine(sourceDir, path::parent(relativePath));
      path::combine(buildDir, path::parent(relativePath));
    }
    m->setSourceDir(sourceDir);
    m->setBuildDir(buildDir);
  }

  TextBuffer * buffer = new TextBuffer();
  m->setTextBuffer(buffer);
  if (!path::readFileContents(absPath, buffer->buffer())) {
    exit(-1);
  }
  buffer->setFilePath(absPath);
  Parser parser(buffer);
  Oper * n = parser.parseModule();
  if (n == NULL || diag::errorCount() > 0) {
    exit(-1);
  }
  Evaluator e(m);
  if (!e.evalModuleContents(n) || diag::errorCount() > 0) {
    exit(-1);
  }
  _modules[String::create(relativePath)] = m;
  return m;
}

void ModuleLoader::findOptions(SmallVectorImpl<Node *> & out) const {
  for (ModuleTable::const_iterator it = _modules.begin(), itEnd = _modules.end(); it != itEnd;
      ++it) {
    const Attributes & properties = it->second->attrs();
    for (Attributes::const_iterator mi = properties.begin(), miEnd = properties.end(); mi != miEnd; ++mi) {
      Object * obj = mi->second->asObject();
      if (obj != NULL && obj->inheritsFrom(TypeRegistry::optionType())) {
        out.push_back(obj);
      }
    }
  }
}

void ModuleLoader::trace() const {
  GC::safeMark(_project);
  GC::safeMark(_prelude);
  _modules.trace();
}

}
