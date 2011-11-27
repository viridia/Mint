/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/project/Configurator.h"
#include "mint/project/Project.h"

#include "mint/support/Diagnostics.h"

namespace mint {

void Configurator::visitObject(Object * obj) {
  //diag::info() << "Visiting " << obj;

  // We need the set of all exported parameters.
  SmallVector<String *, 32> exportNames;
  for (Object * o = obj; o != NULL; o = o->prototype()) {
    // Make certain that object properties have been set.
    if (obj->definition() != NULL) {
      _eval.evalObjectContents(obj);
    }
    for (PropertyTable::const_iterator it = o->properties().begin(), itEnd = o->properties().end();
        it != itEnd; ++it) {
      //diag::info() << "  Checking property " << it->first;
      if (it->second->nodeKind() == Node::NK_PROPDEF) {
        Property * prop = static_cast<Property *>(it->second);
        if (prop->isExport()) {
          //diag::info() << "  Export property " << it->first;
          exportNames.push_back(it->first);
        }
      }
    }
  }

  // If there's anything to export
  if (!exportNames.empty()) {
    // Take the computed property and set them as constants on the object.
    PropertyTable & properties = obj->properties();
    for (SmallVector<String *, 32>::const_iterator
        ex = exportNames.begin(), exEnd = exportNames.end(); ex != exEnd; ++ex) {
      String * name = *ex;
      //diag::info() << "Evaluating property " << name;
      // If the property is not defined on the object directly, then compute it.
      PropertyTable::const_iterator it = properties.find(name);
      if (it == properties.end()) {
        Property * prop = obj->getPropertyDefinition(*name);
        Node * value = _eval.evalObjectProperty(prop->location(), obj, *name);
        if (value != NULL) {
          properties[name] = value;
          visit(value);
        }
      }


//          Node * value = evalObjectProperty(loc, obj, name);
//          obj->properties()[String::createIdent(name)] = value;
//          if (value->nodeKind() == Node::NK_OBJECT) {
//            evalObjectContents(static_cast<Object *>(value));
//          }
//        }
//        return obj->getPropertyValue(name);
//      }


      //Node * value = _eval.realizeObjectProperty(Location(), obj, (*ex)->value());
      //visit(value);
    }
  }

  //obj->dump();
}

}
