/* ================================================================== *
 * Module
 * ================================================================== */

#include "mint/graph/Module.h"
#include "mint/support/OStream.h"

#include "mint/project/Project.h"

namespace mint {

void Module::setProperty(String * name, Node * value) {
  _properties[name] = value;
  _keyOrder.push_back(name);
}

Node * Module::getAttributeValue(StringRef name) const {
  Attributes::const_iterator it = _properties.find_as(name);
  if (it != _properties.end()) {
    return it->second;
  }
  for (ImportList::const_iterator m = _importScopes.end(); m != _importScopes.begin(); ) {
    --m;
    Node * value = (*m)->getAttributeValue(name);
    if (value != NULL) {
      return value;
    }
  }
  return NULL;
}

bool Module::getAttribute(StringRef name, AttributeLookup & result) const {
  Attributes::const_iterator it = _properties.find_as(name);
  if (it != _properties.end()) {
    result.value = it->second;
    result.foundScope = const_cast<Module *>(this);
    result.definition = NULL;
    return true;
  }
  for (ImportList::const_iterator m = _importScopes.end(); m != _importScopes.begin(); ) {
    --m;
    if ((*m)->getAttribute(name, result)) {
      return true;
    }
  }
  return false;
}

void Module::dump() const {
  console::err() << "module " << _moduleName;
  console::err() << " {\n";
  for (Attributes::const_iterator it = _properties.begin(), itEnd = _properties.end(); it != itEnd; ++it) {
    console::err() << "  " << it->first << " = " << it->second << "\n";
  }
  console::err() << "}\n";
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
