/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/project/Configurator.h"
#include "mint/project/Project.h"

#include "mint/support/Diagnostics.h"

namespace mint {

void Configurator::visitObject(Object * obj) {
  // We need the set of all exported parameters.
  SmallVector<String *, 32> exportNames;
  if (obj->definition() != NULL) {
    _eval.evalObjectContents(obj);
  }
  for (Object * o = obj; o != NULL; o = o->prototype()) {
    for (PropertyTable::const_iterator it = o->properties().begin(), itEnd = o->properties().end();
        it != itEnd; ++it) {
      if (it->second->nodeKind() == Node::NK_PROPDEF) {
        Property * prop = static_cast<Property *>(it->second);
        if (prop->isExport()) {
          exportNames.push_back(it->first);
        }
      }
    }
  }

  for (SmallVector<String *, 32>::const_iterator ex = exportNames.begin(), exEnd = exportNames.end(); ex != exEnd; ++ex) {
    _eval.realizeObjectProperty(Location(), obj, (*ex)->value());
  }

  obj->dump();
}

}
