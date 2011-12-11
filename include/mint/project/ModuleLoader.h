/* ================================================================== *
 * ModuleRegistry
 * ================================================================== */

#ifndef MINT_PROJECT_MODULELOADER_H
#define MINT_PROJECT_MODULELOADER_H

#ifndef MINT_GRAPH_MODULE_H
#include "mint/graph/Module.h"
#endif

namespace mint {

class Project;

/** -------------------------------------------------------------------------
    Manages loading and caching of all modules.
 */
class ModuleLoader {
public:
  typedef StringDict<Module> ModuleTable;

  /// Default constructor
  ModuleLoader(StringRef sourceRoot, Project * project)
    : _sourceRoot(sourceRoot)
    , _project(project)
    , _prelude(NULL)
  {
  }

  /// Return the specified module, loading it from 'path' if it has not
  /// already been loaded. 'path' will be resolved relative to the source
  /// root.
  Module * load(StringRef path);

  /// The path to the root of the source tree.
  StringRef sourceRoot() const { return _sourceRoot; }

  /// The standard prelude module.
  Module * prelude() const { return _prelude; }
  void setPrelude(Module * prelude) { _prelude = prelude; }

  /// The map of all modules
  const ModuleTable & modules() const { return _modules; }

  /// Find all options in all the modules we know about.
  void findOptions(SmallVectorImpl<Node *> & out) const;

  /// Garbage collection
  void trace() const;

private:

  StringRef _sourceRoot;
  Project * _project;
  Module * _prelude;
  ModuleTable _modules;
};

}

#endif // MINT_PROJECT_MODULELOADER_H
