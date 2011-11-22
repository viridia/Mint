/* ================================================================== *
 * ModuleLoader
 * ================================================================== */

#include "mint/parse/Parser.h"

#include "mint/eval/Analyzer.h"
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

Module * ModuleLoader::load(StringRef filePath) {
  ModuleTable::const_iterator it = _modules.find_as(filePath);
  if (it != _modules.end()) {
    return it->second;
  }

  SmallString<128> absPath(_sourceRoot);
  path::combine(absPath, filePath);
  if (!path::test(absPath, path::IS_FILE | path::IS_READABLE, false)) {
    exit(-1);
  }

  Module * m = new Module(Node::NK_MODULE, absPath, _project);
  M_ASSERT(_project);
  m->acquire();
  if (_project != NULL && _project->fundamentals() != NULL) {
    m->setParentScope(_project->fundamentals());
  }
  if (_prelude != NULL) {
    m->addImportScope(_prelude.ptr());
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
  _modules[String::create(filePath)] = m;
  return m;
}

void ModuleLoader::findOptions(SmallVectorImpl<Object *> & out) const {
  for (ModuleTable::const_iterator it = _modules.begin(), itEnd = _modules.end(); it != itEnd;
      ++it) {
    const StringDict<Node> & properties = it->second->properties();
    for (StringDict<Node>::const_iterator mi = properties.begin(), miEnd = properties.end(); mi != miEnd; ++mi) {
      Node * n = mi->second;
      if (n->nodeKind() == Node::NK_OPTION) {
        out.push_back(static_cast<Object *>(n));
      }
    }
  }
}

}
