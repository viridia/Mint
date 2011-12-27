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

typedef ArrayRef<StringRef> StringRefArray;
typedef ArrayRef<char *> CStringArray;

/** -------------------------------------------------------------------------
    The built-in root module.
 */
class Project : public GC {
public:
  typedef ModuleLoader::ModuleTable ModuleTable;

  /// Constructor
  Project(BuildConfiguration * buildConfig, String * sourceRoot);

  /// Absolute path to the build directory for this project.
  StringRef buildRoot() const { return _buildRoot->value(); }
  void setBuildRoot(StringRef buildRoot);

  /// Absolute path to the main source directory for this project.
  StringRef sourceRoot() const { return _sourceRoot->value(); }

  /// Get the primary module for this project, loading it if necessary.
  Module * mainModule();

  /// Table of all modules loaded
  const ModuleTable & modules() const { return _modules.modules(); }

  /// Load a module by name within this project.
  Module * loadModule(StringRef name);

  /// The build configuration object.
  BuildConfiguration * buildConfig() const { return _buildConfig; }

  /// Create the objects representing the current project option settings.
  void createOptionDefaults();

  /// Set the values of the project options.
  bool updateOptionValues(ArrayRef<Node *> nodes);

  /// Set the value of a project option.
  bool setOption(StringRef optName, StringRef optValue);

  /// Set the value of multiple project options.
  bool setOptions(CStringArray::const_iterator first, CStringArray::const_iterator last);

  /// Return the value of the specified option.
  Node * optionValue(StringRef str);

  /// Set the project configuration.
  bool setConfig(ArrayRef<Node *> nodes);

  /// Lookup an object by name, in this project or in dependent projects
  Object * lookupObject(StringRef name);

  /// Given a source path, create a target for that path, composing it from the prototype
  /// and the parameter objects. If a target already exists for that path, then return it.
  Object * targetForSource(String * source, Object * proto, Oper * params);

  /// Given an output path, create a target for that path, composing it from the prototype
  /// and the parameter objects. If a target already exists for that path, then return it.
  Object * targetForOutput(String * output, Object * proto, Oper * params);

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

  /// Return the list of project options (names and values) sorted in name order.
  void getProjectOptions(SmallVectorImpl<StringDict<Object>::value_type> & options) const;

  /// Write out makefiles for this project
  void writeMakefiles() const;

private:
  bool setConfigVars(Node * n);

  BuildConfiguration * _buildConfig;
  String * _sourceRoot;
  String * _buildRoot;
  ModuleLoader _modules;
  Module * _mainModule;
  StringDict<Object> _options;
  StringDict<Object> _targetsForSource;
  StringDict<Object> _targetsForOutput;
};

}

#endif // MINT_PROJECT_PROJECT_H
