/* ================================================================== *
 * BuildConfiguration
 * ================================================================== */

#ifndef MINT_PROJECT_BUILDCONFIGURATION_H
#define MINT_PROJECT_BUILDCONFIGURATION_H

#ifndef MINT_INTRINSIC_FUNDAMENTALS_H
#include "mint/intrinsic/Fundamentals.h"
#endif

namespace mint {

class Project;
class Oper;

/** -------------------------------------------------------------------------
    Represents a build directory and the configuration associated with it.
 */
class BuildConfiguration {
public:

  /// Constructor
  BuildConfiguration();

  /// Destructor
  ~BuildConfiguration();

  /// Absolute path to the top-level build directory.
  StringRef buildRoot() const { return _buildRoot; }
  void setBuildRoot(StringRef buildRoot);

  /// The module containing all of the built-in definitions.
  Fundamentals * fundamentals() const { return _fundamentals.ptr(); }

  /// Add a new project definition to this build configuration.
  Project * addSourceProject(StringRef sourcePath, bool mainProject);

  /// The standard prelude.
  Project * prelude() const { return _prelude; }

  /// Read an existing build configuration from the current directory.
  bool readConfig();
  bool readProjectConfig(Oper * project);

  // Mint commands

  /// Initialize a new build configuration in the build directory
  void initialize(ArrayRef<char *> cmdLineArgs);

  /// Print project-specific options
  void showOptions(ArrayRef<char *> cmdLineArgs);

private:
  SmallString<0> _buildRoot;
  Ref<Fundamentals> _fundamentals;
  Project * _mainProject;
  Project * _prelude;
};

}

#endif // MINT_PROJECT_BUILDCONFIGURATION_H
