/* ================================================================== *
 * Project
 * ================================================================== */

#include "mint/build/TargetMgr.h"
#include "mint/build/TargetFinder.h"

#include "mint/eval/Evaluator.h"

#include "mint/graph/Object.h"
#include "mint/graph/GraphWriter.h"

#include "mint/project/BuildConfiguration.h"
#include "mint/project/Configurator.h"
#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"
#include "mint/support/Path.h"

namespace mint {

/** -------------------------------------------------------------------------
    Functor for comparing options.
 */
struct OptionComparator {
  inline bool operator()(const Object * lhs, const Object * rhs) {
    String * lhName = static_cast<String *>(lhs->getAttributeValue("name"));
    String * rhName = static_cast<String *>(rhs->getAttributeValue("name"));
    return lhName->value().compare(rhName->value()) < 0;
  }
};

/** -------------------------------------------------------------------------
    Functor for comparing strings.
 */
struct StringComparator {
  inline bool operator()(const String * lhs, const String * rhs) {
    return lhs->value().compare(rhs->value()) < 0;
  }
};

Project::Project(BuildConfiguration * buildConfig, StringRef sourceRoot)
  : _buildConfig(buildConfig)
  , _buildRoot(NULL)
  , _modules(sourceRoot, this)
  , _mainModule(NULL)
{
  if (buildConfig->prelude()) {
    _modules.setPrelude(buildConfig->prelude()->loadMainModule());
  }
}

void Project::setBuildRoot(StringRef buildRoot) {
  _buildRoot = String::create(buildRoot);
  // We want to ensure that this directory exists and is writeable
  if (!path::test(_buildRoot->value(), path::IS_DIRECTORY | path::IS_WRITABLE, false)) {
    exit(-1);
  }
}

Module * Project::loadMainModule() {
  if (_mainModule == NULL) {
    _mainModule = _modules.load("");
    if (_mainModule == NULL) {
      diag::error() << "Main module for project at '" << sourceRoot() << "' not found";
      exit(-1);
    }
  }
  return _mainModule;
}

Module * Project::loadModule(StringRef name) {
  return _modules.load(name);
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
    M_ASSERT(option->module() != NULL);
    String * optName = String::dyn_cast(option->getAttributeValue("name"));
    String * optHelp = String::dyn_cast(option->getAttributeValue("help"));
//    String * optGroup = String::dyn_cast(option->getAttributeValue("group"));
    //String * optAbbrev = String::dyn_cast(option->getPropertyValue("abbrev"));
    AttributeLookup value;
    option->getAttribute("value", value);
    Type * optType = value.value->type();
    Node * optDefault = option->getAttributeValue("default");

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
    if (value.value != NULL) {
      console::out() << " = " << value.value;
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

void Project::configure() {
  M_ASSERT(_mainModule != NULL) << "No main module defined for project " << _buildRoot;
  GC::sweep();
  Configurator config(this, _mainModule);
  config.visitModule(_mainModule);
  if (diag::errorCount() > 0) {
    return;
  }
  GC::sweep();
  config.performActions(_mainModule);
  if (diag::errorCount() > 0) {
    return;
  }
  GC::sweep();
  GraphWriter writer(console::out());
  writer.write(_mainModule);
  if (diag::errorCount() > 0) {
    return;
  }

  TargetMgr * targetMgr = new TargetMgr();
  TargetFinder finder(targetMgr, this);
  finder.visitModule(_mainModule);
  //GC::sweep();
  if (diag::errorCount() > 0) {
    return;
  }

  console::out() << "Available targets:\n";
  for (TargetMap::const_iterator it = targetMgr->targets().begin(), itEnd = targetMgr->targets().end(); it != itEnd; ++it) {
    String * path = it->second->path();
    if (path != NULL) {
      console::out() << "  " << path->value() << "\n";
      it->second->checkState();
    }
  }
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
    String * optName = String::dyn_cast(option->getAttributeValue("name"));
    //String * optHelp = String::dyn_cast(option->getPropertyValue("help"));
    //String * optAbbrev = String::dyn_cast(option->getPropertyValue("abbrev"));
    Node * optValue = option->getAttributeValue("value");
    //Node * optDefault = option->getPropertyValue("default");
    strm << "  option " << optName << " {\n";
    if (optValue != NULL) {
      strm << "    " << "value = " << optValue << "\n";
    }
    strm << "  }\n";
  }
}

void Project::writeTargets(OStream & strm) const {
  if (_mainModule != NULL) {
    strm << "  targets = [\n";
    //_mainModule->writeTargets(strm, "/project");
    strm << "  ]\n";
  }
}

void Project::trace() const {
  safeMark(_buildRoot);
  _modules.trace();
  safeMark(_mainModule);
}

}
