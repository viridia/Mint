/* ================================================================== *
 * Module
 * ================================================================== */

#include "mint/graph/Module.h"
#include "mint/graph/Object.h"

#include "mint/intrinsic/TypeRegistry.h"

#include "mint/project/Project.h"

#include "mint/support/OStream.h"

namespace mint {

/// Default constructor
Module::Module(NodeKind kind, StringRef moduleName, Project * project)
  : Node(kind, Location(), TypeRegistry::moduleType())
  , _moduleName(moduleName)
  , _parentScope(NULL)
  , _textBuffer(NULL)
  , _project(project)
{
}

void Module::setAttribute(String * name, Node * value) {
  _attributes[name] = value;
  _keyOrder.push_back(name);
}

Node * Module::getAttributeValue(StringRef name) const {
  Attributes::const_iterator it = _attributes.find_as(name);
  if (it != _attributes.end()) {
    return it->second;
  }
  for (ImportList::const_iterator m = _importScopes.end(); m != _importScopes.begin(); ) {
    --m;
    Node * value = (*m)->getAttributeValue(name);
    if (value != NULL) {
      return value;
    }
  }
  return Node::getAttributeValue(name);
}

bool Module::getAttribute(StringRef name, AttributeLookup & result) const {
  Attributes::const_iterator it = _attributes.find_as(name);
  if (it != _attributes.end()) {
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
  return Node::getAttribute(name, result);
}

void Module::dump() const {
  console::err() << "module " << _moduleName;
  console::err() << " {\n";
  for (Attributes::const_iterator it = _attributes.begin(), itEnd = _attributes.end();
      it != itEnd; ++it) {
    console::err() << "  " << it->first << " = " << it->second << "\n";
  }
  console::err() << "}\n";
}

void Module::trace() const {
  Node::trace();
  markArray(ArrayRef<Node *>(_importScopes));
  markArray(ArrayRef<Node *>(_actions));
  _attributes.trace();
  markArray(ArrayRef<String *>(_keyOrder));
  safeMark(_parentScope);
  safeMark(_textBuffer);
  safeMark(_project);
}

}
