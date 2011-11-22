/* ================================================================== *
 * Module
 * ================================================================== */

#include "mint/graph/Module.h"

namespace mint {

void Module::setProperty(String * name, Node * value) {
  _properties[name] = value;
}

Node * Module::getPropertyValue(String * name) const {
  StringDict<Node>::const_iterator it = _properties.find(name);
  if (it != _properties.end()) {
    return it->second;
  }
  for (ImportList::const_iterator m = _importScopes.end(); m != _importScopes.begin(); ) {
    --m;
    it = (*m)->_properties.find(name);
    if (it != (*m)->_properties.end()) {
      return it->second;
    }
  }
  return NULL;
}

}
