/* ================================================================== *
 * Module
 * ================================================================== */

#include "mint/graph/Module.h"

#include "mint/project/Project.h"

namespace mint {

void Module::setProperty(String * name, Node * value) {
  _properties[name] = value;
  _keyOrder.push_back(name);
}

Node * Module::getPropertyValue(StringRef name) const {
  PropertyTable::const_iterator it = _properties.find_as(name);
  if (it != _properties.end()) {
    return it->second;
  }
  for (ImportList::const_iterator m = _importScopes.end(); m != _importScopes.begin(); ) {
    --m;
    Node * value = (*m)->getPropertyValue(name);
    if (value != NULL) {
      return value;
    }
  }
  return NULL;
}

void Module::trace() const {
  Node::trace();
  markArray(ArrayRef<Node *>(_importScopes));
  markArray(ArrayRef<Node *>(_actions));
  _properties.trace();
  markArray(ArrayRef<String *>(_keyOrder));
  safeMark(_parentScope);
  safeMark(_textBuffer);
  safeMark(_project);
}

}
