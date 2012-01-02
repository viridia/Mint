/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/project/OptionFinder.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"

namespace mint {

OptionFinder::OptionFinder(Project * project)
  : _project(project)
{
  _optionProto = TypeRegistry::optionType();
}

void OptionFinder::visitModules() {
  const Project::ModuleTable & modules = _project->modules();
  for (Project::ModuleTable::const_iterator
      it = modules.begin(), itEnd = modules.end(); it != itEnd; ++it) {
    visitModule(it->second);
  }
}

void OptionFinder::visitObject(Object * obj) {
  // Only visit objects once
  if (_visited.find(obj) != _visited.end()) {
    return;
  }
  _visited[obj] = NULL;

  if (obj->inheritsFrom(_optionProto)) {
    _options.push_back(obj);
  }
}

}
