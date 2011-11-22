/* ================================================================== *
 * GraphBuilder
 * ================================================================== */

#include "mint/graph/GraphBuilder.h"
#include "mint/graph/Oper.h"
#include "mint/graph/Type.h"

#include "mint/support/Assert.h"

namespace mint {

Node * GraphBuilder::createList(Location loc, Type * type) {
  return createList(loc, type, NodeArray());
}

Node * GraphBuilder::createList(Location loc, Type * type, Node * e0) {
  Node *elements[1] = { e0 };
  return createList(loc, type, elements);
}

Node * GraphBuilder::createList(Location loc, Type * type, NodeArray elements) {
  M_ASSERT(type != NULL);
  M_ASSERT(type->typeKind() == Type::LIST);
  Type * elementType = static_cast<DerivedType *>(type)->params()[0];
  for (NodeArray::const_iterator it = elements.begin(); it != elements.end(); ++it) {
    // Check for type compatibility.
  }
  return Oper::create(Node::NK_LIST, loc, type, elements);
}

Node * GraphBuilder::createListOf(Location loc, Type * elementType) {
  return createList(loc, _typeRegistry.getListType(elementType), NodeArray());
}

Node * GraphBuilder::createListOf(Location loc, Type * elementType, Node * e0) {
  return createList(loc, _typeRegistry.getListType(elementType), e0);
}

Node * GraphBuilder::createListOf(Location loc, Type * elementType, NodeArray elements) {
  return createList(loc, _typeRegistry.getListType(elementType), elements);
}

/// Create a call operation with an array of arguments
Node * GraphBuilder::createCall(Location loc, Function * func, NodeArray args) {
  M_ASSERT(func != NULL);
  M_ASSERT(func->type() != NULL);
  M_ASSERT(func->type()->typeKind() == Type::FUNCTION);
  return Oper::create(Node::NK_CALL, loc, static_cast<DerivedType *>(func->type())->params()[0], args);
}

Node * GraphBuilder::createCall(Location loc, Function * func) {
  Node *args[1] = { func };
  return createCall(loc, func, args);
}

Node * GraphBuilder::createCall(Location loc, Function * func, Node * arg0) {
  Node *args[2] = { func, arg0 };
  return createCall(loc, func, args);
}

Node * GraphBuilder::createCall(Location loc, Function * func, Node * arg0, Node * arg1) {
  Node *args[3] = { func, arg0, arg1 };
  return createCall(loc, func, args);
}

Function * GraphBuilder::createFunction(Location loc, Type * returnType, MethodHandler * m) {
  return createFunction(loc, returnType, TypeArray(), m);
}

Function * GraphBuilder::createFunction(Location loc, Type * returnType, Type * arg0Type,
    MethodHandler * m) {
  M_ASSERT(arg0Type != NULL);
  Type *args[1] = { arg0Type };
  return createFunction(loc, returnType, args, m);
}

Function * GraphBuilder::createFunction(Location loc, Type * returnType, TypeArray argTypes,
    MethodHandler * m) {
  M_ASSERT(returnType != NULL);
  M_ASSERT(m != NULL);
  DerivedType * functionType = _typeRegistry.getFunctionType(returnType, argTypes);
  return new Function(Node::NK_FUNCTION, loc, functionType, m);
}

}
