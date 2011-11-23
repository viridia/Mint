/* ================================================================== *
 * mint tool
 * ================================================================== */

#include "mint/project/BuildConfiguration.h"

#include "mint/support/OStream.h"
#include "mint/support/Path.h"

using namespace mint;

void showHelp() {
  using namespace console;
  out() << "Usage: mint <command> [options...]\n";
  out() << "\n";
  out() << "Commands:\n";
  out() << "  options <source-dir>   Show project-specific options.\n";
  out() << "  init <source-dir>      Initialize a new project build configuration.\n";
  out() << "  config                 Run configuration tests and prepare targets for building.\n";
  out() << "  build [<target> ...]   Build the specified targets in the current project.\n";
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
          console::err() << "Required source directory argument missing.\n";
          exit(-1);
        }
      } else if (arg == "options") {
        foundCommand = true;
        if (index < argc) {
          // They specified a source project
          SmallString<128> sourceDir(cwd);
          ::path::combine(sourceDir, argv[index++]);
          bc->addSourceProject(sourceDir, true);
          bc->showOptions(makeArrayRef(&argv[index], &argv[argc]));
        } else if (bc->readConfig()) {
          // They didn't specify a source project, but we found a build configuration
          bc->showOptions(makeArrayRef(&argv[index], &argv[argc]));
        } else {
          console::err() << "Required source directory argument missing.\n";
          exit(-1);
        }
      } else if (arg == "config") {
        foundCommand = true;
        bc->configure(makeArrayRef(&argv[index], &argv[argc]));
      } else if (arg == "build") {
        foundCommand = true;
      } else {
        console::err() << "Unknown command '" << arg << "'. Run 'mint help' for usage.\n";
        exit(-1);
      }
      break;
    }
  }

  if (!foundCommand) {
    console::err() << "No command given. Run 'mint help' for usage.\n";
    exit(-1);
  }

  return index;
}

int main(int argc, char *argv[]) {
  // Get the current dir as the build directory
  SmallVector<char, 256> cwd;
  path::getCurrentDir(cwd);

  // Create the project and set the build directory.
  BuildConfiguration * bc = new BuildConfiguration();
  bc->setBuildRoot(cwd);

  // Parse input parameters.
  parseInputParams(bc, cwd, argc, argv);
  return 0;
}
