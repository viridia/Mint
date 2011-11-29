/* ================================================================== *
 * Object
 * ================================================================== */

#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Object.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

// -------------------------------------------------------------------------
// Object
// -------------------------------------------------------------------------

bool Object::inheritsFrom(Object * proto) const {
  for (const Object * o = this; o != NULL; o = o->_prototype) {
    if (o == proto) {
      return true;
    }
  }
  return false;
}

Property * Object::defineProperty(String * name, Node * value, Type * type, unsigned lazy) {
  Property * p = new Property(value, type, lazy);
  _properties[name] = p;
  return p;
}

Node * Object::getPropertyValue(StringRef name) const {
  for (const Object * ob = this; ob != NULL; ob = ob->prototype()) {
    PropertyTable::const_iterator it = ob->_properties.find_as(name);
    if (it != ob->_properties.end()) {
      return it->second;
    }
  }
  return NULL;
}

Property * Object::getPropertyDefinition(StringRef name) const {
  for (const Object * o = this; o != NULL; o = o->prototype()) {
    PropertyTable::const_iterator it = o->_properties.find_as(name);
    if (it != o->_properties.end()) {
      if (it->second->nodeKind() == Node::NK_PROPDEF) {
        return static_cast<Property *>(it->second);
      }
    }
  }
  return NULL;
}

bool Object::hasPropertyImmediate(StringRef name) const {
  return _properties.find_as(name) != _properties.end();
}

void Object::defineMethod(StringRef name, Type * returnType, MethodHandler * m) {
  defineMethod(name, returnType, TypeArray(), m);
}

void Object::defineMethod(StringRef name, Type * returnType, Type * a0, MethodHandler * m) {
  defineMethod(name, returnType, makeArrayRef(a0), m);
}

void Object::defineMethod(
    StringRef name, Type * returnType, Type * a0, Type * a1, MethodHandler * m) {
  Type * args[] = { a0, a1 };
  defineMethod(name, returnType, args, m);
}

void Object::defineMethod(
    StringRef name, Type * returnType, Type * a0, Type * a1, Type * a2, MethodHandler * m) {
  Type * args[] = { a0, a1, a2 };
  defineMethod(name, returnType, args, m);
}

void Object::defineMethod(
    StringRef name, Type * returnType, TypeArray args, MethodHandler * m) {
  M_ASSERT(returnType != NULL);
  M_ASSERT(m != NULL);
  DerivedType * functionType = TypeRegistry::get().getFunctionType(returnType, args);
  Function * method = new Function(Node::NK_FUNCTION, Location(), functionType, m);
  PropertyTable::const_iterator it = _properties.find_as(name);
  if (it != _properties.end()) {
    diag::error() << "Method '" << name << "' is already defined on '" << this << "'";
  }
  String * methodName = StringRegistry::str(name);
  _properties[methodName] = method;
}

void Object::print(OStream & strm) const {
  if (name()) {
    strm << name();
  } else if (prototype()) {
    strm << prototype();
  } else {
    strm << "<object>";
  }
}

void Object::dump() const {
  if (this->name() != NULL) {
    console::err() << this->name();
  }
  if (_prototype != NULL) {
    console::err() << " = " << _prototype->name();
  }
  console::err() << " {\n";
  for (PropertyTable::const_iterator it = _properties.begin(), itEnd = _properties.end(); it != itEnd; ++it) {
    console::err() << "  " << it->first << " = " << it->second << "\n";
  }
  console::err() << "}\n";
}

void Object::trace() const {
  Node::trace();
  safeMark(_definition);
  safeMark(_name);
  safeMark(_prototype);
  safeMark(_parentScope);
  _properties.trace();
}

}
