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

Module * ModuleLoader::load(StringRef path) {
  ModuleTable::const_iterator it = _modules.find_as(path);
  if (it != _modules.end()) {
    return it->second;
  }

  SmallString<128> absPath(_sourceRoot);
  if (!path.empty()) {
    path::combine(absPath, path);
  }

  bool isDir = false;
  if (path::test(absPath, path::IS_DIRECTORY, true)) {
    path::combine(absPath, "module.mint");
    isDir = true;
  } else {
    path::changeExtension(absPath, "mint");
  }

  if (!path::test(absPath, path::IS_FILE | path::IS_READABLE, true)) {
    diag::error() << "Module '" << absPath << "' not found";
    exit(-1);
  }

  Module * m = new Module(path, _project);
  M_ASSERT(_project);
  m->setParentScope(&Fundamentals::get());
  if (_prelude != NULL) {
    m->addImportScope(_prelude);

    // If the prelude has been loaded, then we should know the source and build dirs too.
    SmallString<128> sourceDir(_sourceRoot);
    SmallString<128> buildDir(_project->buildRoot());
    if (isDir) {
      path::combine(sourceDir, path);
      path::combine(buildDir, path);
    } else {
      path::combine(sourceDir, path::parent(path));
      path::combine(buildDir, path::parent(path));
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
  Oper * n = parser.parseModule(m);
  if (n == NULL || diag::errorCount() > 0) {
    exit(-1);
  }
  Evaluator e(m);
  if (!e.evalModuleContents(n) || diag::errorCount() > 0) {
    exit(-1);
  }
  _modules[String::create(path)] = m;
  return m;
}

void ModuleLoader::findOptions(SmallVectorImpl<Object *> & out) const {
  for (ModuleTable::const_iterator it = _modules.begin(), itEnd = _modules.end(); it != itEnd;
      ++it) {
    const Attributes & properties = it->second->attrs();
    for (Attributes::const_iterator mi = properties.begin(), miEnd = properties.end(); mi != miEnd; ++mi) {
      Node * n = mi->second;
      if (n->nodeKind() == Node::NK_OPTION) {
        out.push_back(static_cast<Object *>(n));
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
