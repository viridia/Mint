/* ================================================================== *
 * Project
 * ================================================================== */

#include "mint/parse/Parser.h"

#include "mint/graph/Oper.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/Project.h"

#include "mint/intrinsic/Fundamentals.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"
#include "mint/support/TextBuffer.h"

namespace mint {

extern char * SRC_PRELUDE_PATH;

BuildConfiguration::BuildConfiguration()
  : _fundamentals(NULL)
  , _mainProject(NULL)
  , _prelude(NULL)
{
  _fundamentals = new Fundamentals();
  M_ASSERT(_fundamentals->nodeKind() == Node::NK_MODULE);
  if (!path::test(SRC_PRELUDE_PATH, path::IS_DIRECTORY | path::IS_READABLE, false)) {
    exit(-1);
  }
  _prelude = new Project(this, SRC_PRELUDE_PATH);
  _prelude->loadMainModule();
}

BuildConfiguration::~BuildConfiguration() {
}

void BuildConfiguration::setBuildRoot(StringRef buildRoot) {
  _buildRoot = buildRoot;
  // We want to ensure that this directory exists and is writeable
  if (!path::test(_buildRoot, path::IS_DIRECTORY | path::IS_WRITABLE, false)) {
    exit(-1);
  }
}

bool BuildConfiguration::readConfig() {
  SmallString<128> absPath(_buildRoot);
  path::combine(absPath, "build.mint");
  if (!path::test(absPath, path::IS_FILE | path::IS_READABLE, true)) {
    // No configuration found
    return false;
  }

  TextBuffer * buffer = new TextBuffer();
  if (!path::readFileContents(absPath, buffer->buffer())) {
    exit(-1);
  }
  buffer->setFilePath(absPath);
  Parser parser(buffer);
  Oper * config = parser.parseConfig();
  if (config == NULL || diag::errorCount() > 0) {
    exit(-1);
  }

  for (NodeArray::const_iterator it = config->begin(), itEnd = config->end(); it != itEnd; ++it) {
    Node * n = *it;
    switch (n->nodeKind()) {
      case Node::NK_PROJECT: {
        if (!readProjectConfig(static_cast<Oper *>(n))) {
          return false;
        }
        break;
      }

      default:
        diag::error(n->location()) << "Invalid node type for build configuration: "
            << n->nodeKind();
        exit(-1);
        break;
    }
  }

  return true;
}

bool BuildConfiguration::readProjectConfig(Oper * proj) {
  for (NodeArray::const_iterator ni = proj->begin(), niEnd = proj->end(); ni != niEnd; ++ni) {
    Node * n = *ni;
    switch (n->nodeKind()) {
      case Node::NK_SET_MEMBER: {
        Oper * op = static_cast<Oper *>(n);
        String * propName = String::cast(op->arg(0));
        Node * propValue = op->arg(1);
        if (propName->value() == "source_dir") {
          String * dir = String::cast(propValue);
          addSourceProject(dir->value(), _mainProject == NULL);
        }
        break;
      }

      case Node::NK_MAKE_OPTION:
        //n->dump();
        break;

      default:
        diag::error(n->location()) << "Invalid node type for project configuration: "
            << n->nodeKind();
        return false;
    }
  }
  return true;
}

Project * BuildConfiguration::addSourceProject(StringRef sourcePath, bool mainProject) {
  //sourcePath = _alloc.makeString(sourcePath);
  // We want to ensure that this directory exists and is writeable
  if (!path::test(sourcePath, path::IS_DIRECTORY | path::IS_READABLE, false)) {
    exit(-1);
  }

  Project * result = new Project(this, sourcePath);
  if (mainProject == true) {
    M_ASSERT(_mainProject == NULL) << "main project has already been set!";
    _mainProject = result;
  } else {
    //M_ASSERT(false) << "Implement non-main projects";
  }

  result->loadMainModule();
  return result;
}

void BuildConfiguration::initialize(ArrayRef<char *> cmdLineArgs) {
  SmallString<128> buildFilePath(_buildRoot);
  path::combine(buildFilePath, "build.mint");
  OFileStream strm(buildFilePath);
  if (_mainProject != NULL) {
    _mainProject->writeProjectInfo(strm);
  }
}

void BuildConfiguration::showOptions(ArrayRef<char *> cmdLineArgs) {
  if (!cmdLineArgs.empty()) {
    diag::warn(Location()) << "Additional input parameters ignored.";
  }
  if (_mainProject != NULL) {
    _mainProject->showOptions();
  }
}

}
