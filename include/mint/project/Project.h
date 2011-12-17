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
class String;

/** -------------------------------------------------------------------------
    The built-in root module.
 */
class Project : public GC {
public:

  /// Constructor
  Project(BuildConfiguration * buildConfig, String * sourceRoot);

  /// Absolute path to the build directory for this project.
  StringRef buildRoot() const { return _buildRoot->value(); }
  void setBuildRoot(StringRef buildRoot);

  /// Absolute path to the main source directory for this project.
  StringRef sourceRoot() const { return _sourceRoot->value(); }

  /// Get the primary module for this project, loading it if necessary.
  Module * mainModule();

  /// Load a module by name within this project.
  Module * loadModule(StringRef name);

  /// The build configuration object.
  BuildConfiguration * buildConfig() const { return _buildConfig; }

  /// Create the objects representing the current project option settings.
  void setProjectOptions();

  /// Set the values of the project options.
  bool setOptionValues(ArrayRef<Node *> nodes);

  /// Set the project configuration.
  bool setConfig(ArrayRef<Node *> nodes);

  /// Lookup an object by name, in this project or in dependent projects
  Object * lookupObject(StringRef name);

  // Mint commands

  /// Print out all project options.
  void showOptions() const;

  /// Configure this project.
  void configure();

  /// Do configuration actions
  void generate();

  /// Collect targets into the target manager.
  void gatherTargets();

  /// Garbage collection
  void trace() const;

  /// Write out the current value of all build options
  void writeOptions(GraphWriter & writer) const;

  /// Write out the configuration
  void writeConfig(GraphWriter & writer) const;

private:
  void getProjectOptions(SmallVectorImpl<StringDict<Object>::value_type> & options) const;
  bool setConfigVars(Node * n);

  BuildConfiguration * _buildConfig;
  String * _sourceRoot;
  String * _buildRoot;
  ModuleLoader _modules;
  Module * _mainModule;
  StringDict<Object> _options;
};

}

#endif // MINT_PROJECT_PROJECT_H
