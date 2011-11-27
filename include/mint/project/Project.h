/* ================================================================== *
 * MintProject
 * ================================================================== */

#ifndef MINT_PROJECT_PROJECT_H
#define MINT_PROJECT_PROJECT_H

#ifndef MINT_PROJECT_MODULELOADER_H
#include "mint/project/ModuleLoader.h"
#endif

#ifndef MINT_GRAPH_TYPEREGISTRY_H
#include "mint/graph/TypeRegistry.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLSTRING_H
#include "mint/collections/SmallString.h"
#endif

namespace mint {

class Object;
class BuildConfiguration;
class Fundamentals;

/** -------------------------------------------------------------------------
    The built-in root module.
 */
class Project : public GC {
public:

  /// Constructor
  Project(BuildConfiguration * buildConfig, StringRef sourceRoot);

  /// Absolute path to the build directory for this project.
  StringRef buildRoot() const { return _buildRoot->value(); }
  void setBuildRoot(StringRef buildRoot);

  /// Absolute path to the main source directory for this project.
  StringRef sourceRoot() const { return _modules.sourceRoot(); }

  /// Load the primary module for this project.
  Module * loadMainModule();

  /// Load a module by name within this project.
  Module * loadModule(StringRef name);

  /// The module containing the built-in definitions.
  Fundamentals * fundamentals() const;

  /// The build configuration object.
  BuildConfiguration * buildConfig() const { return _buildConfig; }

  // Mint commands

  /// Print out all project options.
  void showOptions() const;

  /// Configure this project.
  void configure();

  /// Write out the current value of all build options
  void writeProjectInfo(OStream & strm) const;
  void writeOptions(OStream & strm) const;
  void writeTargets(OStream & strm) const;

  /// Garbage collection
  void trace() const;

private:
  void readProject();

  BuildConfiguration * _buildConfig;
  String * _buildRoot;
  ModuleLoader _modules;
  Module * _mainModule;
};

}

#endif // MINT_PROJECT_PROJECT_H
