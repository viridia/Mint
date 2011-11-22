/* ================================================================== *
 * Function
 * ================================================================== */

#include "mint/graph/Function.h"
#include "mint/graph/Type.h"

namespace mint {

Function::Function(NodeKind nk, Location loc, Type * type, MethodHandler * handler)
  : Node(nk, loc, type)
  , _handler(handler)
{
}

Type * Function::returnType() const {
  DerivedType * dt = static_cast<DerivedType *>(type());
  return dt->params()[0];
}

unsigned Function::argCount() const {
  DerivedType * dt = static_cast<DerivedType *>(type());
  return dt->params().size() - 1;
}

Type * Function::argType(unsigned index) const {
  DerivedType * dt = static_cast<DerivedType *>(type());
  return dt->params()[index + 1];
}

//Node * Function::eval(Object * object, NodeArray args) {
//  return NULL;
//}

}
