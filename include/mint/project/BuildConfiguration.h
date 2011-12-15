/* ================================================================== *
 * BuildConfiguration
 * ================================================================== */

#ifndef MINT_PROJECT_BUILDCONFIGURATION_H
#define MINT_PROJECT_BUILDCONFIGURATION_H

#ifndef MINT_SUPPORT_GC_H
#include "mint/support/GC.h"
#endif

#ifndef MINT_COLLECTIONS_SMALLSTRING_H
#include "mint/collections/SmallString.h"
#endif

#ifndef MINT_GRAPH_STRINGDICT_H
#include "mint/graph/StringDict.h"
#endif

namespace mint {

class Fundamentals;
class Project;
class Oper;
class JobMgr;
class TargetMgr;
class Directory;

/** -------------------------------------------------------------------------
    Represents a build directory and the configuration associated with it.
 */
class BuildConfiguration : public GCRootBase {
public:

  /// Constructor
  BuildConfiguration();

  /// Destructor
  ~BuildConfiguration();

  /// Absolute path to the top-level build directory.
  StringRef buildRoot() const { return _buildRoot; }
  void setBuildRoot(StringRef buildRoot);

  /// Add a new project definition to this build configuration.
  Project * addSourceProject(StringRef sourcePath, bool mainProject);

  /// The standard prelude.
  Project * prelude() const { return _prelude; }

  /// Locate a project by name.
  Project * getProject(StringRef name);

  /// Write out the project options
  void writeOptions();

  /// Read an existing build options from the current directory.
  /// If 'required' is true, and options cannot be read, an error will be produced.
  bool readOptions(bool required = true);

  /// Write out the build configuration
  void writeConfig();

  /// Read an existing build configuration from the current directory.
  bool readConfig();

  /// Return the target manager
  TargetMgr * targetMgr();

  /// Return the job manager
  JobMgr * jobMgr();

  // Mint commands

  /// Initialize a new build configuration in the build directory
  void initialize(ArrayRef<char *> cmdLineArgs);

  /// Print project-specific options
  void showOptions(ArrayRef<char *> cmdLineArgs);

  /// Run configuration tests and prepare all targets for building.
  void configure(ArrayRef<char *> cmdLineArgs);

  /// Generate build files for the specified build system.
  void generate(ArrayRef<char *> cmdLineArgs);

  /// Build the specified targets
  void build(ArrayRef<char *> cmdLineArgs);

  /// Remove output files
  void clean(ArrayRef<char *> cmdLineArgs);

  /// Show all targets
  void showTargets(ArrayRef<char *> cmdLineArgs);

  /// Trace roots
  void trace() const;

private:
  bool readProjects(StringRef file, SmallVectorImpl<Node *> & projects, bool required);
  void createSubdirs(Directory * dir);

  SmallString<0> _buildRoot;
  Fundamentals * _fundamentals;
  StringDict<Project> _projects;
  Project * _mainProject;
  Project * _prelude;
  TargetMgr * _targetMgr;
  JobMgr * _jobMgr;
};

}

#endif // MINT_PROJECT_BUILDCONFIGURATION_H
