/* ================================================================== *
 * Project
 * ================================================================== */

#include "mint/build/TargetMgr.h"

#include "mint/parse/Parser.h"

#include "mint/graph/GraphWriter.h"
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

static const char * BUILD_FILE = "build.mint";
static const char * CONFIG_FILE = "config.mint";

extern char * SRC_PRELUDE_PATH;

BuildConfiguration::BuildConfiguration()
  : _fundamentals(NULL)
  , _mainProject(NULL)
  , _prelude(NULL)
  , _targetMgr(NULL)
{
  _fundamentals = new Fundamentals();
  M_ASSERT(_fundamentals->nodeKind() == Node::NK_MODULE);
  if (!path::test(SRC_PRELUDE_PATH, path::IS_DIRECTORY | path::IS_READABLE, false)) {
    exit(-1);
  }
  _prelude = new Project(this, SRC_PRELUDE_PATH);
  _prelude->mainModule();
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

void BuildConfiguration::writeOptions() {
  SmallString<128> buildFilePath(_buildRoot);
  path::combine(buildFilePath, BUILD_FILE);
  OFileStream strm(buildFilePath);
  GraphWriter writer(strm);
  if (_mainProject != NULL) {
    _mainProject->makeProjectOptions();
    _mainProject->writeOptions(writer);
  }
}

bool BuildConfiguration::readOptions() {
  SmallString<128> absPath(_buildRoot);
  path::combine(absPath, BUILD_FILE);
  if (!path::test(absPath, path::IS_FILE | path::IS_READABLE, true)) {
    // No options file found
    return false;
  }

  TextBuffer * buffer = new TextBuffer();
  if (!path::readFileContents(absPath, buffer->buffer())) {
    exit(-1);
  }
  buffer->setFilePath(absPath);
  Parser parser(buffer);
  Parser::NodeList projects;
  if (!parser.parseOptions(projects) || diag::errorCount() > 0) {
    exit(-1);
  }

  if (projects.empty()) {
    diag::error() << "No projects found in 'build.mint'.";
    exit(-1);
  }

  for (NodeArray::const_iterator it = projects.begin(), itEnd = projects.end(); it != itEnd; ++it) {
    Node * n = *it;
    switch (n->nodeKind()) {
      case Node::NK_PROJECT: {
        if (!readProjectOptions(static_cast<Oper *>(n))) {
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

bool BuildConfiguration::readProjectOptions(Oper * proj) {
  NodeArray::const_iterator ni = proj->begin(), niEnd = proj->end();
  String * sourceDir = String::cast(*ni++);
  addSourceProject(sourceDir->value(), _mainProject == NULL);
  for (; ni != niEnd; ++ni) {
    Node * n = *ni;
    switch (n->nodeKind()) {
      case Node::NK_SET_MEMBER: {
        Oper * op = static_cast<Oper *>(n);
        String * propName = String::cast(op->arg(0));
        Node * propValue = op->arg(1);
        (void)propName;
        (void)propValue;
        break;
      }

      default:
        diag::error(n->location()) << "Invalid node type for project configuration: "
            << n->nodeKind();
        return false;
    }
  }
  return true;
}

void BuildConfiguration::writeConfig() {
  SmallString<128> buildFilePath(_buildRoot);
  path::combine(buildFilePath, BUILD_FILE);
  OFileStream strm(buildFilePath);
  if (_mainProject != NULL) {
    GraphWriter writer(console::out());
    writer.write(_mainProject->mainModule());
  }
}

bool BuildConfiguration::readConfig() {
  SmallString<128> absPath(_buildRoot);
  path::combine(absPath, CONFIG_FILE);
  if (!path::test(absPath, path::IS_FILE | path::IS_READABLE, true)) {
    // No configuration file found
    return false;
  }

  TextBuffer * buffer = new TextBuffer();
  if (!path::readFileContents(absPath, buffer->buffer())) {
    exit(-1);
  }
  buffer->setFilePath(absPath);
  Parser parser(buffer);
#if 0
  Oper * config = parser.parseConfig();
  if (config == NULL || diag::errorCount() > 0) {
    exit(-1);
  }

  for (NodeArray::const_iterator it = config->begin(), itEnd = config->end(); it != itEnd; ++it) {
    Node * n = *it;
    switch (n->nodeKind()) {
      case Node::NK_PROJECT: {
        if (!readProjectOptions(static_cast<Oper *>(n))) {
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
#endif

  return true;
}

Project * BuildConfiguration::addSourceProject(StringRef sourcePath, bool mainProject) {
  // We want to ensure that this directory exists and is writeable
  if (!path::test(sourcePath, path::IS_DIRECTORY | path::IS_READABLE, false)) {
    exit(-1);
  }

  Project * result = new Project(this, sourcePath);
  if (mainProject == true) {
    M_ASSERT(_mainProject == NULL) << "main project has already been set!";
    _mainProject = result;
    _mainProject->setBuildRoot(buildRoot());
  } else {
    //M_ASSERT(false) << "Implement non-main projects";
  }

  result->mainModule();
  return result;
}

Project * BuildConfiguration::getProject(StringRef name) {
  if (name == "prelude") {
    return prelude();
  }
  M_ASSERT(false) << "implement";
  return NULL;
}

void BuildConfiguration::initialize(ArrayRef<char *> cmdLineArgs) {
  writeOptions();
}

void BuildConfiguration::showOptions(ArrayRef<char *> cmdLineArgs) {
  if (!cmdLineArgs.empty()) {
    diag::warn(Location()) << "Additional input parameters ignored.";
  }
  if (_mainProject != NULL) {
    _mainProject->makeProjectOptions();
    _mainProject->showOptions();
  }
}

void BuildConfiguration::configure(ArrayRef<char *> cmdLineArgs) {
  if (!readOptions()) {
    M_ASSERT(false) << "No build configuration!";
  }
  M_ASSERT(_mainProject != NULL);
  _mainProject->makeProjectOptions();
  _mainProject->configure();
  if (diag::errorCount() == 0) {
    writeConfig();
  }
}

void BuildConfiguration::generate(ArrayRef<char *> cmdLineArgs) {
}

void BuildConfiguration::build(ArrayRef<char *> cmdLineArgs) {
}

void BuildConfiguration::showTargets(ArrayRef<char *> cmdLineArgs) {
}

void BuildConfiguration::trace() const {
  GC::safeMark(_fundamentals);
  GC::safeMark(_mainProject);
  GC::safeMark(_prelude);
  GC::safeMark(_targetMgr);
}

}
