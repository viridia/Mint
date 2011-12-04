/* ================================================================== *
 * Object
 * ================================================================== */

#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

// -------------------------------------------------------------------------
// Object
// -------------------------------------------------------------------------

// TODO: Deprecate this and use type only.
Object * Object::prototype() const {
  return type() && type()->nodeKind() == NK_OBJECT ? static_cast<Object *>(type()) : NULL;
}

bool Object::inheritsFrom(Object * proto) const {
  for (const Node * o = this; o != NULL; o = o->type()) {
    if (o == proto) {
      return true;
    }
  }
  return false;
}

void Object::setName(StringRef name) {
  setName(StringRegistry::str(name));
}

AttributeDefinition * Object::defineAttribute(String * name, Node * value, Type * type, int flags) {
  AttributeDefinition * p = new AttributeDefinition(value, type, flags);
  _attrs[name] = p;
  return p;
}

AttributeDefinition * Object::defineDynamicAttribute(
    StringRef name, Type * type, MethodHandler * mh) {
  TypeRegistry & typeReg = TypeRegistry::get();
  String * attrName = StringRegistry::str(name);
  DerivedType * functionType = typeReg.getFunctionType(type);
  Function * func = new Function(Node::NK_FUNCTION, Location(), functionType, mh);
  Node *args[] = { func, this };
  Node * call = Oper::create(Node::NK_DEFERRED, Location(), type, args);
  AttributeDefinition * attrDef = new AttributeDefinition(call, type, true);
  _attrs[attrName] = attrDef;
  return attrDef;
}

Node * Object::getAttributeValue(StringRef name) const {
  for (const Node * ob = this; ob != NULL; ob = ob->type()) {
    if (ob->nodeKind() >= Node::NK_OBJECTS_FIRST && ob->nodeKind() <= Node::NK_OBJECTS_LAST) {
      const Attributes & attrs = static_cast<const Object *>(ob)->_attrs;
      Attributes::const_iterator it = attrs.find_as(name);
      if (it != attrs.end()) {
        return it->second;
      }
    }
  }
  return NULL;
}

bool Object::getAttribute(StringRef name, AttributeLookup & result) const {
  for (const Node * ob = this; ob != NULL; ob = ob->type()) {
    if (ob->nodeKind() >= Node::NK_OBJECTS_FIRST && ob->nodeKind() <= Node::NK_OBJECTS_LAST) {
      const Attributes & attrs = static_cast<const Object *>(ob)->_attrs;
      Attributes::const_iterator it = attrs.find_as(name);
      if (it != attrs.end()) {
        Node * n = it->second;
        if (n->nodeKind() == Node::NK_PROPDEF) {
          result.definition = static_cast<AttributeDefinition *>(n);
          if (result.value == NULL) {
            result.value = result.definition->value();
            result.foundScope = const_cast<Node *>(ob);
          }
          break;
        } else {
          result.foundScope = const_cast<Node *>(ob);
          result.value = n;
        }
      }
    }
  }
  return result.value != NULL;
}

bool Object::hasPropertyImmediate(StringRef name) const {
  return _attrs.find_as(name) != _attrs.end();
}

Node * Object::getElement(Node * index) const {
  if (String * str = String::dyn_cast(index)) {
    return getAttributeValue(str->value());
  }
  diag::error(index->location()) << "Invalid key type: " << index->type();
  return &Node::UNDEFINED_NODE;
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
  Attributes::const_iterator it = _attrs.find_as(name);
  if (it != _attrs.end()) {
    diag::error() << "Method '" << name << "' is already defined on '" << this << "'";
  }
  String * methodName = StringRegistry::str(name);
  _attrs[methodName] = method;
}

Object * Object::makeDict(Object * prototype, StringRef name) {
  Object * result = new Object(NK_DICT, Location(), prototype);
  if (!name.empty()) {
    result->setName(name);
  }
  return result;
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
  if (prototype() != NULL && prototype()->name() != NULL) {
    console::err() << " = " << prototype()->name();
  }
  console::err() << " {\n";
  for (Attributes::const_iterator it = _attrs.begin(), itEnd = _attrs.end(); it != itEnd; ++it) {
    console::err() << "  " << it->first << " = " << it->second << "\n";
  }
  console::err() << "}\n";
}

void Object::trace() const {
  Node::trace();
  safeMark(_definition);
  safeMark(_name);
  safeMark(_parentScope);
  _attrs.trace();
}

}
