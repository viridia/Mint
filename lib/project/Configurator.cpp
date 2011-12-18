/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/project/Configurator.h"
#include "mint/project/Project.h"

#include "mint/support/Diagnostics.h"

namespace mint {

void Configurator::performActions(Module * module) {
  for (Module::ActionList::const_iterator
      it = module->actions().begin(), itEnd = module->actions().end(); it != itEnd; ++it) {
    Node * action = *it;
    visit(action);
  }
}

void Configurator::visitObject(Object * obj) {
  // We need the set of all cacheded parameters.
  SmallVector<String *, 32> cachedNames;
  for (Object * o = obj; o != NULL && o != TypeRegistry::objectType(); o = o->prototype()) {
    // Make certain that object attributes have been set.
    _eval.ensureObjectContents(obj);
    for (Attributes::const_iterator it = o->attrs().begin(), itEnd = o->attrs().end(); it != itEnd;
        ++it) {
      if (it->second->nodeKind() == Node::NK_ATTRDEF) {
        AttributeDefinition * prop = static_cast<AttributeDefinition *>(it->second);
        if (prop->isCached()) {
          cachedNames.push_back(it->first);
        }
      }
    }
  }

  // If there's anything to cached
  if (!cachedNames.empty()) {
    // Take the computed attribute and set them as constants on the object.
    Attributes & attributes = obj->attrs();
    for (SmallVector<String *, 32>::const_iterator
        ex = cachedNames.begin(), exEnd = cachedNames.end(); ex != exEnd; ++ex) {
      String * name = *ex;
      // If the attribute is not defined on the object directly, then compute it.
      // TODO: we should probably store these elsewhere.
      Attributes::const_iterator it = attributes.find(name);
      if (it == attributes.end()) {
        AttributeLookup lookup;
        obj->getAttribute(*name, lookup);
        Node * value = _eval.evalAttribute(lookup.value->location(), lookup, obj, *name);
        if (value != NULL) {
          attributes[name] = value;
          visit(value);
        }
      }
    }
  }
}

}
