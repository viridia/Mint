/* ================================================================== *
 * mint tool
 * ================================================================== */

#include "mint/project/BuildConfiguration.h"

#include "mint/support/CommandLine.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/GC.h"
#include "mint/support/Path.h"

using namespace mint;

cl::OptionGroup global("global", "Global program options");
cl::OptionGroup debugging("debug", "Options for debugging");

cl::Option<bool> help("help", cl::Group("global"), cl::Description("Display this message."));

void showHelp() {
  using namespace console;
  out() << "Usage: mint [global-options...] <command> [options...]\n";
  out() << "\n";
  out() << "Commands:\n";
  out() << "  options <source-dir>    Show project-specific options.\n";
  out() << "  init <source-dir>       Initialize a new project build configuration.\n";
  out() << "  config                  Run configuration tests and prepare targets for building.\n";
  out() << "  targets                 List all buildable targets.\n";
  out() << "  build [<target> ...]    Build the specified targets in the current project.\n";
  out() << "  clean                   Delete output files of all targets.\n";
  out() << "  generate <builder-type> Generate build files for the specified build system.\n";
  out() << "  help                    Display usage information.\n";
  out() << "  help [topic]            Show help on a specific topic or command.\n";
  out() << "                          Topics are: 'global' for help on global options.\n";
}

void parseInputParams(BuildConfiguration * bc, StringRef cwd, int argc, char *argv[]) {
  bool foundCommand = false;
  char ** ai = &argv[1];
  char ** aiEnd = &argv[argc];

  StringRef groups[] = { "global", "debug" };
  // Process global flags
  ai = cl::Parser::parse(groups, ai, aiEnd);

  if (help) {
    showHelp();
    return;
  }

  while (ai < aiEnd) {
    StringRef arg = *ai++;
    if (arg == "help") {
      foundCommand = true;
      if (ai < aiEnd) {
        StringRef topic = *ai++;
        cl::showHelp(topic);
        return;
      }
      showHelp();
    } else if (arg == "init") {
      foundCommand = true;
      if (ai < aiEnd) {
        // They specified a source project
        SmallString<128> sourceDir(cwd);
        ::path::combine(sourceDir, *ai++);
        bc->addSourceProject(sourceDir, true);
        bc->initialize(makeArrayRef(ai, aiEnd));
      } else {
        diag::error() << "Required source directory argument missing.";
        exit(-1);
      }
    } else if (arg == "options") {
      foundCommand = true;
      if (ai < aiEnd) {
        // They specified a source project
        SmallString<128> sourceDir(cwd);
        ::path::combine(sourceDir, *ai++);
        bc->addSourceProject(sourceDir, true);
      }
      bc->showOptions(makeArrayRef(ai, aiEnd));
    } else if (arg == "set") {
      foundCommand = true;
      bc->setOptions(makeArrayRef(ai, aiEnd));
    } else if (arg == "config") {
      foundCommand = true;
      bc->configure(makeArrayRef(ai, aiEnd));
    } else if (arg == "generate") {
      foundCommand = true;
      bc->generate(makeArrayRef(ai, aiEnd));
    } else if (arg == "build") {
      foundCommand = true;
      bc->build(makeArrayRef(ai, aiEnd));
    } else if (arg == "clean") {
      foundCommand = true;
      bc->clean(makeArrayRef(ai, aiEnd));
    } else if (arg == "targets") {
      foundCommand = true;
      bc->showTargets(makeArrayRef(ai, aiEnd));
    } else if (arg == "dump") {
      foundCommand = true;
      bc->dumpTargets(makeArrayRef(ai, aiEnd));
    } else {
      // No command recognized, so attempt to match against a target name
      --ai;
      bc->build(makeArrayRef(ai, aiEnd));
      foundCommand = true;
    }
    break;
  }

  if (!foundCommand) {
    diag::error() << "No command given. Run 'mint help' for usage.";
    exit(-1);
  }
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
