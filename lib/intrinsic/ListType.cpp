/* ================================================================== *
 * List
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Function.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

Node * methodListMap(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  M_ASSERT(self->nodeKind() == Node::NK_LIST);
  Node * mapFn = args[0];
  Oper * list = static_cast<Oper *>(self);
  SmallVector<Node *, 64> result;
  result.resize(list->size());
  SmallVector<Node *, 64>::iterator out = result.begin();
  Type * elementType = NULL;
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = ex->call(loc, mapFn, NULL, makeArrayRef(*it));
    elementType = ex->selectCommonType(elementType, n->type());
    *out++ = n;
  }
  if (elementType == NULL) {
    elementType = TypeRegistry::anyType();
  }
  return Oper::create(
      Node::NK_LIST, fn->location(), ex->typeRegistry().getListType(elementType), result);
}

Node * methodListFilter(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  M_ASSERT(self->nodeKind() == Node::NK_LIST);
  Node * filterFn = args[0];
  Oper * list = static_cast<Oper *>(self);
  SmallVector<Node *, 64> result;
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = ex->call(loc, filterFn, NULL, makeArrayRef(*it));
    if (!ex->isNonNil(n)) {
      result.push_back(n);
    }
  }
  return Oper::create(Node::NK_LIST, fn->location(), list->type(), result);
}

void initListMethods(Fundamentals * fundamentals) {
  DerivedType * mapFunctionType = TypeRegistry::get().getFunctionType(
      TypeRegistry::anyType(), TypeRegistry::anyType());
  DerivedType * filterFunctionType = TypeRegistry::get().getFunctionType(
      TypeRegistry::boolType(), TypeRegistry::anyType());

  fundamentals->list = new Object(Node::NK_DICT, Location(), NULL);
  fundamentals->list->setName(fundamentals->str("list"));
  fundamentals->list->defineMethod(
      "map", TypeRegistry::genericListType(), mapFunctionType, methodListMap);
  fundamentals->list->defineMethod(
      "filter", TypeRegistry::genericListType(), filterFunctionType, methodListFilter);
}

}
