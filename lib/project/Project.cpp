/* ================================================================== *
 * Project
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/graph/Object.h"
#include "mint/graph/GraphWriter.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"

namespace mint {

/** -------------------------------------------------------------------------
    Functor for comparing options.
 */
struct OptionComparator {
  inline bool operator()(const Object * lhs, const Object * rhs) {
    String * lhName = static_cast<String *>(lhs->getPropertyValue("name"));
    String * rhName = static_cast<String *>(rhs->getPropertyValue("name"));
    return lhName->value().compare(rhName->value()) < 0;
  }
};

Project::Project(BuildConfiguration * buildConfig, StringRef sourceRoot)
  : _buildConfig(buildConfig)
  , _modules(sourceRoot, this)
  , _mainModule(NULL)
{
  if (buildConfig->prelude()) {
    _modules.setPrelude(buildConfig->prelude()->loadMainModule());
  }
}

Project::~Project() {
}

void Project::setBuildRoot(StringRef sourceRoot) {
  _buildRoot = String::create(sourceRoot);
  // We want to ensure that this directory exists and is writeable
  if (!path::test(_buildRoot->value(), path::IS_DIRECTORY | path::IS_WRITABLE, false)) {
    exit(-1);
  }
}

Module * Project::loadMainModule() {
  if (_mainModule == NULL) {
    _mainModule = _modules.load("module.mint");
    M_ASSERT(_mainModule != NULL);
  }
  return _mainModule.ptr();
}

Fundamentals * Project::fundamentals() const {
  if (_buildConfig != NULL) {
    return _buildConfig->fundamentals();
  }
  return NULL;
}

void Project::showOptions() const {
  // Search for options in project modules.
  SmallVector<Object *, 32> options;
  _modules.findOptions(options);

  // Sort options by name
  std::sort(options.begin(), options.end(), OptionComparator());
  console::out() << "Project options:\n";

  bool isTerminal = console::out().isTerminal();
  for (SmallVectorImpl<Object *>::const_iterator it = options.begin(), itEnd = options.end();
      it != itEnd; ++it) {
    Object * option = (*it);
    Type * optType = option->type();
    String * optName = String::dyn_cast(option->getPropertyValue("name"));
    String * optHelp = String::dyn_cast(option->getPropertyValue("help"));
    //String * optAbbrev = String::dyn_cast(option->getPropertyValue("abbrev"));
    Node * optValue = option->getPropertyValue("value");
    Node * optDefault = option->getPropertyValue("default");

    // Convert underscores to dashes.
    SmallString<32> name(optName->value());
    for (SmallVectorImpl<char>::iterator it = name.begin(), itEnd = name.end(); it != itEnd; ++it) {
      if (*it == '_') {
        *it = '-';
      }
    }

    // Print out the option
    if (isTerminal) {
      console::out().changeColor(OStream::GREEN, true);
    }
    console::out() << "  " << name;
    if (isTerminal) {
      console::out().resetColor();
    }
    if (optType != NULL) {
      console::out() << " : " << optType;
    }
    if (optValue != NULL) {
      console::out() << " = " << optValue;
    } else if (optDefault != NULL) {
      console::out() << " [default = " << optDefault << "]";
    }
    console::out() << "\n";

    if (optHelp) {
      if (isTerminal) {
        console::out().changeColor(OStream::CYAN, false);
      }
      console::out().indent(6);
      console::out() << optHelp->value() << "\n";
      if (isTerminal) {
        console::out().resetColor();
      }
    }
  }
}

void Project::configure() const {
  M_ASSERT(_mainModule.ptr() != NULL) << "No main module defined for project " << _buildRoot.ptr();
  Evaluator ev(_mainModule.ptr());
  const StringDict<Node> & properties = _mainModule->properties();
  for (StringDict<Node>::const_iterator it = properties.begin(), itEnd = properties.end();
      it != itEnd; ++it) {
    Node * n = it->second;
    if (n->nodeKind() == Node::NK_OBJECT) {
      Object * obj = static_cast<Object *>(n);
      if (obj->definition() != NULL) {
        ev.evalObjectContents(obj);
      }
    }
  }
  GraphWriter writer(console::out());
  writer.write(_mainModule.ptr());
}

void Project::writeProjectInfo(OStream & strm) const {
  strm << "project {\n";
  // TODO: Need to escape this string.
  strm << "  source_dir = \"" << sourceRoot() << "\"\n";
  writeOptions(strm);
  //writeTargets(strm);
  strm << "}\n";
}

void Project::writeOptions(OStream & strm) const {
  // Search for options in project modules.
  SmallVector<Object *, 32> options;
  _modules.findOptions(options);

  // Sort options by name
  std::sort(options.begin(), options.end(), OptionComparator());
  for (SmallVectorImpl<Object *>::const_iterator it = options.begin(), itEnd = options.end();
      it != itEnd; ++it) {
    Object * option = (*it);
    //Type * optType = option->type();
    String * optName = String::dyn_cast(option->getPropertyValue("name"));
    //String * optHelp = String::dyn_cast(option->getPropertyValue("help"));
    //String * optAbbrev = String::dyn_cast(option->getPropertyValue("abbrev"));
    Node * optValue = option->getPropertyValue("value");
    //Node * optDefault = option->getPropertyValue("default");
    strm << "  option " << optName << " {\n";
    if (optValue != NULL) {
      strm << "    " << "value = " << optValue << "\n";
    }
    strm << "  }\n";
  }
}

void Project::writeTargets(OStream & strm) const {
  if (_mainModule.ptr() != NULL) {
    strm << "  targets = [\n";
    _mainModule->writeTargets(strm, "/project");
    strm << "  ]\n";
  }
}

}
