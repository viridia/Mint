/* ================================================================== *
 * Function
 * ================================================================== */

#include "mint/graph/Function.h"
#include "mint/graph/Type.h"

#include "mint/support/OStream.h"

namespace mint {

Function::Function(NodeKind nk, Location loc, Type * type, MethodHandler * handler,
    unsigned flags)
  : Node(nk, loc, type)
  , _name(NULL)
  , _handler(handler)
  , _parentScope(NULL)
  , _body(NULL)
  , _flags(flags)
{
}

Function::Function(NodeKind nk, Location loc, Type * type,
    const SmallVectorImpl<Parameter> & params, MethodHandler * handler,
    unsigned flags)
  : Node(nk, loc, type)
  , _name(NULL)
  , _handler(handler)
  , _parentScope(NULL)
  , _params(params.begin(), params.end())
  , _body(NULL)
  , _flags(flags)
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

void Function::trace() const {
  Node::trace();
  safeMark(_name);
  safeMark(_parentScope);
  safeMark(_body);
  traceArray(ArrayRef<Parameter>(_params));
}

void Function::print(OStream & strm) const {
  if (_name != NULL) {
    strm << _name;
  } else {
    strm << "<unnamed_function>";
  }
  // TODO: Arguments?
}

}
