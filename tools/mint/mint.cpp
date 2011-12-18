/* ================================================================== *
 * mint tool
 * ================================================================== */

#include "mint/project/BuildConfiguration.h"

#include "mint/support/Diagnostics.h"
#include "mint/support/GC.h"
#include "mint/support/Path.h"

using namespace mint;

void showHelp() {
  using namespace console;
  out() << "Usage: mint <command> [options...]\n";
  out() << "\n";
  out() << "Commands:\n";
  out() << "  options <source-dir>    Show project-specific options.\n";
  out() << "  init <source-dir>       Initialize a new project build configuration.\n";
  out() << "  config                  Run configuration tests and prepare targets for building.\n";
  out() << "  targets                 List all buildable targets.\n";
  out() << "  build [<target> ...]    Build the specified targets in the current project.\n";
  out() << "  clean                   Delete output files of all targets.\n";
  out() << "  generate <builder-type> Generate build files for the specified build system.\n";
}

int parseInputParams(BuildConfiguration * bc, StringRef cwd, int argc, char *argv[]) {
  int index = 1;
  bool foundCommand = false;
  while (index < argc) {
    StringRef arg = argv[index++];
    if (arg.startsWith("--")) {
      arg = arg.substr(2);
      if (arg == "help") {
        showHelp();
        break;
      }
    } else if (arg.startsWith("-")) {
      arg = arg.substr(1);
      // TODO: what?
    } else {
      if (arg == "help") {
        foundCommand = true;
        showHelp();
      } else if (arg == "init") {
        foundCommand = true;
        if (index < argc) {
          // They specified a source project
          SmallString<128> sourceDir(cwd);
          ::path::combine(sourceDir, argv[index++]);
          bc->addSourceProject(sourceDir, true);
          bc->initialize(makeArrayRef(&argv[index], &argv[argc]));
        } else {
          diag::error() << "Required source directory argument missing.";
          exit(-1);
        }
      } else if (arg == "options") {
        foundCommand = true;
        if (index < argc) {
          // They specified a source project
          SmallString<128> sourceDir(cwd);
          ::path::combine(sourceDir, argv[index++]);
          bc->addSourceProject(sourceDir, true);
        }
        bc->showOptions(makeArrayRef(&argv[index], &argv[argc]));
      } else if (arg == "config") {
        foundCommand = true;
        bc->configure(makeArrayRef(&argv[index], &argv[argc]));
      } else if (arg == "generate") {
        foundCommand = true;
        bc->generate(makeArrayRef(&argv[index], &argv[argc]));
      } else if (arg == "build") {
        foundCommand = true;
        bc->build(makeArrayRef(&argv[index], &argv[argc]));
      } else if (arg == "clean") {
        foundCommand = true;
        bc->clean(makeArrayRef(&argv[index], &argv[argc]));
      } else if (arg == "targets") {
        foundCommand = true;
        bc->showTargets(makeArrayRef(&argv[index], &argv[argc]));
      } else {
        diag::error() << "Unknown command '" << arg << "'. Run 'mint help' for usage.";
        exit(-1);
      }
      break;
    }
  }

  if (!foundCommand) {
    diag::error() << "No command given. Run 'mint help' for usage.";
    exit(-1);
  }

  return index;
}

int main(int argc, char *argv[]) {
  // Get the current dir as the build directory
  SmallVector<char, 256> cwd;
  path::getCurrentDir(cwd);

  // Initialize the garbage collector
  GC::init();
  //GC::setDebugLevel(1);

  // Create the project and set the build directory.
  BuildConfiguration * bc = new BuildConfiguration();
  bc->setBuildRoot(cwd);

  // Parse input parameters.
  parseInputParams(bc, cwd, argc, argv);
  GC::uninit();
  return 0;
}
