/* ================================================================== *
 * Project
 * ================================================================== */

#include "mint/build/JobMgr.h"
#include "mint/build/TargetMgr.h"

#include "mint/parse/Parser.h"

#include "mint/graph/GraphWriter.h"
#include "mint/graph/Oper.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/Project.h"
#include "mint/project/ProjectWriterXml.h"

#include "mint/intrinsic/Fundamentals.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"
#include "mint/support/TextBuffer.h"

namespace mint {

static const char * BUILD_FILE = "build.mint";
static const char * CONFIG_FILE = "config.mint";

#ifdef SRCDIR_PRELUDE_PATH
const char * SRC_PRELUDE_PATH = SRCDIR_PRELUDE_PATH;
#else
extern char * SRC_PRELUDE_PATH;
#endif

BuildConfiguration::BuildConfiguration()
  : _fundamentals(NULL)
  , _mainProject(NULL)
  , _prelude(NULL)
  , _targetMgr(NULL)
  , _jobMgr(NULL)
{
  M_ASSERT(_prelude == NULL);
  _fundamentals = new Fundamentals();
  M_ASSERT(_fundamentals->nodeKind() == Node::NK_MODULE);
  if (!path::test(SRC_PRELUDE_PATH, path::IS_DIRECTORY | path::IS_READABLE, false)) {
    exit(-1);
  }
  M_ASSERT(_prelude == NULL);
  _prelude = new Project(this, String::create(SRC_PRELUDE_PATH));
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

Project * BuildConfiguration::addSourceProject(StringRef sourcePath, bool mainProject) {
  String * sourceDir = String::create(sourcePath);
  StringDict<Project>::const_iterator it = _projects.find(sourceDir);
  if (it != _projects.end()) {
    return it->second;
  }

  // We want to ensure that this directory exists and is writeable
  if (!path::test(sourcePath, path::IS_DIRECTORY | path::IS_READABLE, false)) {
    exit(-1);
  }

  Project * result = new Project(this, sourceDir);
  if (mainProject == true) {
    M_ASSERT(_mainProject == NULL) << "main project has already been set!";
    _mainProject = result;
    _mainProject->setBuildRoot(buildRoot());
  } else {
    //M_ASSERT(false) << "Implement non-main projects";
  }

  _projects[sourceDir] = result;
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

TargetMgr * BuildConfiguration::targetMgr() {
  if (_targetMgr == NULL) {
    _targetMgr = new TargetMgr();
    if (!_buildRoot.empty()) {
      _targetMgr->setBuildRoot(_buildRoot);
    }
  }
  return _targetMgr;
}

JobMgr * BuildConfiguration::jobMgr() {
  if (_jobMgr == NULL) {
    _jobMgr = new JobMgr(targetMgr());
  }
  return _jobMgr;
}

void BuildConfiguration::writeOptions() {
  diag::status() << "Writing project options\n";
  SmallString<128> buildFilePath(_buildRoot);
  path::combine(buildFilePath, BUILD_FILE);
  OFileStream strm(buildFilePath);
  GraphWriter writer(strm);
  if (_mainProject != NULL) {
    _mainProject->writeOptions(writer);
  }
}

bool BuildConfiguration::readOptions(bool required) {
  Parser::NodeList projects;
  if (!readProjects(BUILD_FILE, projects, required)) {
    if (required) {
      exit(-1);
    }
    return false;
  }

  for (NodeArray::const_iterator it = projects.begin(), itEnd = projects.end(); it != itEnd; ++it) {
    Node * n = *it;
    switch (n->nodeKind()) {
      case Node::NK_PROJECT: {
        Oper * projNode = static_cast<Oper *>(n);
        NodeArray::const_iterator ni = projNode->begin(), niEnd = projNode->end();
        String * sourceDir = String::cast(*ni++);
        Project * proj = addSourceProject(sourceDir->value(), _mainProject == NULL);
        if (!proj->updateOptionValues(makeArrayRef(ni, niEnd))) {
          return false;
        }
        break;
      }

      default:
        diag::error(n->location()) << "Invalid node type for project options: " << n->nodeKind();
        exit(-1);
        break;
    }
  }

  M_ASSERT(_mainProject != NULL);
  _mainProject->createOptionDefaults();
  return true;
}

void BuildConfiguration::writeConfig() {
  SmallString<128> buildFilePath(_buildRoot);
  path::combine(buildFilePath, CONFIG_FILE);
  OFileStream strm(buildFilePath);
  if (_mainProject != NULL) {
    GraphWriter writer(strm);
    writer.setIncludeOptions(false);
    writer.setIncludeTargets(false);
    _mainProject->writeConfig(writer);
  }
}

bool BuildConfiguration::readConfig() {
  Parser::NodeList projects;
  if (!readProjects(CONFIG_FILE, projects, true)) {
    return false;
  }
  for (NodeArray::const_iterator it = projects.begin(), itEnd = projects.end(); it != itEnd; ++it) {
    Node * n = *it;
    switch (n->nodeKind()) {
      case Node::NK_PROJECT: {
        Oper * projNode = static_cast<Oper *>(n);
        NodeArray::const_iterator ni = projNode->begin(), niEnd = projNode->end();
        String * sourceDir = String::cast(*ni++);
        Project * proj = addSourceProject(sourceDir->value(), _mainProject == NULL);
        if (!proj->setConfig(makeArrayRef(ni, niEnd))) {
          return false;
        }
        break;
      }

      default:
        diag::error(n->location()) << "Invalid node type for project config: " << n->nodeKind();
        exit(-1);
        break;
    }
  }
  return true;
}

void BuildConfiguration::initialize(CStringArray cmdLineArgs) {
  if (_mainProject != NULL) {
    _mainProject->createOptionDefaults();
    writeOptions();
  }
}

void BuildConfiguration::showOptions(CStringArray cmdLineArgs) {
  if (!cmdLineArgs.empty()) {
    diag::warn(Location()) << "Additional input parameters ignored.";
  }
  if (_mainProject == NULL) {
    readOptions();
  } else {
    _mainProject->createOptionDefaults();
  }
  if (diag::errorCount() == 0 && _mainProject != NULL) {
    _mainProject->showOptions();
  }
}

void BuildConfiguration::setOptions(CStringArray cmdLineArgs) {
  if (_mainProject == NULL) {
    readOptions();
  } else {
    _mainProject->createOptionDefaults();
  }
  if (diag::errorCount() == 0 && _mainProject != NULL) {
    _mainProject->setOptions(cmdLineArgs.begin(), cmdLineArgs.end());
    if (diag::errorCount() == 0) {
      writeOptions();
    }
  }
}

void BuildConfiguration::configure(CStringArray cmdLineArgs) {
  if (!cmdLineArgs.empty()) {
    diag::warn(Location()) << "Additional input parameters ignored.";
  }
  readOptions();
  _mainProject->configure();
  _mainProject->generate();
  _mainProject->gatherTargets();
  GC::sweep();
  if (diag::errorCount() == 0) {
    writeConfig();
    createSubdirs(_targetMgr->buildRoot());
    GC::sweep();
  }
}

void BuildConfiguration::generate(CStringArray cmdLineArgs) {
  CStringArray::const_iterator ai = cmdLineArgs.begin(), aiEnd = cmdLineArgs.end();
  bool makeFormat = false;
  bool xmlFormat = false;
  while (ai < aiEnd) {
    StringRef arg = *ai++;
    if (arg == "makefile") {
      makeFormat = true;
    } else if (arg == "xml") {
      xmlFormat = true;
    } else {
      diag::error() << "Unrecognized generator format: '" << arg << "'.";
      return;
    }
    break;
  }

  if (ai < aiEnd) {
    diag::warn(Location()) << "Additional input parameters ignored.";
  }

  readOptions();
  if (!readConfig()) {
    exit(-1);
  }
  _mainProject->configure();
  _mainProject->gatherTargets();
  GC::sweep();
  if (diag::errorCount() == 0) {
    if (xmlFormat) {
      ProjectWriterXml projectWriter(console::out());
      projectWriter.writeBuildConfiguration(this);
    } else if (makeFormat) {
      _mainProject->writeMakefiles();
    }
  }
}

void BuildConfiguration::build(CStringArray cmdLineArgs) {
  readOptions();
  if (!readConfig()) {
    exit(-1);
  }
  _mainProject->configure();
  _mainProject->gatherTargets();
  if (diag::errorCount() != 0) {
    return;
  }
  GC::sweep();
  createSubdirs(_targetMgr->buildRoot());
  GC::sweep();

  JobMgr * jm = jobMgr();
  if (diag::errorCount() == 0) {
    bool all = true;

    for (CStringArray::const_iterator
        it = cmdLineArgs.begin(), itEnd = cmdLineArgs.end(); it != itEnd; ++it) {
      char * arg = *it;
      Object * obj = _mainProject->lookupObject(arg);
      Target * target = obj != NULL ? _targetMgr->getTarget(obj, false) : NULL;
      if (target != NULL) {
        jm->addReady(target);
      } else {
        diag::error() << "No such target: " << arg;
      }
      all = false;
    }

    if (all) {
      jm->addAllReady();
    }
  }
  if (diag::errorCount() == 0) {
    jm->run();
  }
}

void BuildConfiguration::clean(CStringArray cmdLineArgs) {
  if (!cmdLineArgs.empty()) {
    diag::warn(Location()) << "Additional input parameters ignored.";
  }
  readOptions();
  if (!readConfig()) {
    exit(-1);
  }
  _mainProject->configure(); // TODO: load configuration
  _mainProject->gatherTargets();
  GC::sweep();
  if (diag::errorCount() == 0) {
    targetMgr()->deleteOutputFiles();
  }
}

void BuildConfiguration::showTargets(CStringArray cmdLineArgs) {
  if (!cmdLineArgs.empty()) {
    diag::warn(Location()) << "Additional input parameters ignored.";
  }
  if (!readOptions()) {
    M_ASSERT(false) << "No build configuration!";
  }
  M_ASSERT(_mainProject != NULL);
  readConfig();
  _mainProject->configure();
  _mainProject->gatherTargets();
  GC::sweep();
  if (diag::errorCount() == 0) {
    diag::status() << "Available targets:\n";
    for (TargetMap::const_iterator it =
        _targetMgr->targets().begin(), itEnd = _targetMgr->targets().end(); it != itEnd; ++it) {
      Target * target = it->second;
      if (!target->isSourceOnly() && !target->isInternal()) {
        String * path = target->path();
        if (path != NULL) {
          console::out() << "  " << path->value() << "\n";
        }
      }
    }
    GC::sweep();
  }
}

void BuildConfiguration::dumpTargets(CStringArray cmdLineArgs) {
  if (!readOptions()) {
    M_ASSERT(false) << "No build configuration!";
  }
  M_ASSERT(_mainProject != NULL);
  readConfig();
  _mainProject->configure();
  _mainProject->gatherTargets();
  GC::sweep();
  if (diag::errorCount() == 0) {
    for (CStringArray::const_iterator
        it = cmdLineArgs.begin(), itEnd = cmdLineArgs.end(); it != itEnd; ++it) {
      char * arg = *it;
      Object * obj = _mainProject->lookupObject(arg);
      Target * target = obj != NULL ? _targetMgr->getTarget(obj, false) : NULL;
      if (target != NULL) {
        target->checkState();
        target->definition()->dump();
      } else {
        diag::error() << "No such target: " << arg;
      }
    }
  }
}

bool BuildConfiguration::readProjects(
    StringRef file, SmallVectorImpl<Node *> & projects, bool required) {
  SmallString<128> absPath(_buildRoot);
  path::combine(absPath, file);
  if (!path::test(absPath, path::IS_FILE | path::IS_READABLE, true)) {
    // No configuration file found
    if (required) {
      diag::error() << "Missing project configuration: '" << file << "'.";
    }
    return false;
  }

  TextBuffer * buffer = new TextBuffer();
  if (!path::readFileContents(absPath, buffer->buffer())) {
    return false;
  }
  buffer->setFilePath(absPath);
  Parser parser(buffer);
  if (!parser.parseProjects(projects) || diag::errorCount() > 0) {
    return false;
  }

  if (projects.empty()) {
    diag::error() << "No projects found in '" << file << "'.";
    return false;
  }

  return true;
}

void BuildConfiguration::createSubdirs(Directory * dir) {
  for (Directory::Directories::const_iterator
      it = dir->subdirs().begin(), itEnd = dir->subdirs().end(); it != itEnd; ++it) {
    Directory * subdir = it->second;
    subdir->updateDirectoryStatus();
    if (!subdir->exists()) {
      if (!subdir->create()) {
        break;
      }
    }
    createSubdirs(subdir);
  }
}

void BuildConfiguration::trace() const {
  GC::safeMark(_fundamentals);
  _projects.trace();
  GC::safeMark(_mainProject);
  GC::safeMark(_prelude);
  GC::safeMark(_targetMgr);
  GC::safeMark(_jobMgr);
}

}
