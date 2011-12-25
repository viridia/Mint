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
Module::Module(StringRef moduleName, Project * project)
  : Object(NK_MODULE, Location(), TypeRegistry::moduleType())
  , _textBuffer(NULL)
  , _project(project)
{
  setName(String::create(moduleName));
}

void Module::setAttribute(String * name, Node * value) {
  _attrs[name] = value;
  _keyOrder.push_back(name);
}

Node * Module::getAttributeValue(StringRef name) const {
  Attributes::const_iterator it = _attrs.find_as(name);
  if (it != _attrs.end()) {
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
  Attributes::const_iterator it = _attrs.find_as(name);
  if (it != _attrs.end()) {
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
  StringRef moduleName = *name();
  if (moduleName.empty()) {
    moduleName = "<main>";
  }
  console::err() << "module " << moduleName;
  console::err() << " {\n";
  for (Attributes::const_iterator it = _attrs.begin(), itEnd = _attrs.end();
      it != itEnd; ++it) {
    console::err() << "  " << it->first << " = " << it->second << "\n";
  }
  console::err() << "}\n";
}

void Module::print(OStream & strm) const {
  StringRef moduleName = *name();
  if (moduleName.empty()) {
    moduleName = "<main>";
  }
  console::err() << "module " << moduleName;
}

void Module::trace() const {
  Object::trace();
  markArray(ArrayRef<Object *>(_importScopes));
  markArray(ArrayRef<Node *>(_actions));
  markArray(ArrayRef<String *>(_keyOrder));
  safeMark(_textBuffer);
  safeMark(_project);
}

}
