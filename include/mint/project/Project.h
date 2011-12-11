/* ================================================================== *
 * MintProject
 * ================================================================== */

#ifndef MINT_PROJECT_PROJECT_H
#define MINT_PROJECT_PROJECT_H

#ifndef MINT_PROJECT_MODULELOADER_H
#include "mint/project/ModuleLoader.h"
#endif

#ifndef MINT_INTRINSIC_TYPEREGISTRY_H
#include "mint/intrinsic/TypeRegistry.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLSTRING_H
#include "mint/collections/SmallString.h"
#endif

namespace mint {

class Object;
class BuildConfiguration;
class GraphWriter;

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

  /// Get the primary module for this project, loading it if necessary.
  Module * mainModule();

  /// Load a module by name within this project.
  Module * loadModule(StringRef name);

  /// The build configuration object.
  BuildConfiguration * buildConfig() const { return _buildConfig; }

  /// Create the objects representing the current project option settings.
  void makeProjectOptions();

  // Mint commands

  /// Print out all project options.
  void showOptions() const;

  /// Configure this project.
  void configure();

  /// Write out the current value of all build options
  void writeOptions(GraphWriter & writer) const;

  /// Garbage collection
  void trace() const;

private:
  void writeProjectConfig(GraphWriter & writer) const;
  void readProject();
  void getProjectOptions(SmallVectorImpl<StringDict<Object>::value_type> & options) const;

  BuildConfiguration * _buildConfig;
  String * _buildRoot;
  ModuleLoader _modules;
  Module * _mainModule;
  StringDict<Object> _options;
};

}

#endif // MINT_PROJECT_PROJECT_H
