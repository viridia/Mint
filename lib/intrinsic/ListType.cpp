/* ================================================================== *
 * List
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"

#include "mint/graph/GraphBuilder.h"
#include "mint/graph/Function.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

Node * methodListMap(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  M_ASSERT(self->nodeKind() == Node::NK_LIST);
  M_ASSERT(args[0]->nodeKind() == Node::NK_FUNCTION);
  Function * mapFn = static_cast<Function *>(args[0]);
  Oper * list = static_cast<Oper *>(self);
  SmallVector<Node *, 64> result;
  result.resize(list->size());
  SmallVector<Node *, 64>::iterator out = result.begin();
  Type * elementType = NULL;
  for (Oper::const_iterator it = list->begin(), itEnd = list->end(); it != itEnd; ++it) {
    Node * n = (*mapFn->handler())(ex, mapFn, NULL, makeArrayRef(*it));
    elementType = ex->selectCommonType(elementType, n->type());
    *out++ = n;
  }
  if (elementType == NULL) {
    elementType = TypeRegistry::anyType();
  }
  return Oper::create(
      Node::NK_LIST, fn->location(), ex->typeRegistry().getListType(elementType), result);
}

void initListType(Fundamentals * fundamentals) {
  GraphBuilder builder(fundamentals->typeRegistry());
  DerivedType * mapFunctionType = fundamentals->typeRegistry().getFunctionType(
      TypeRegistry::anyType(), TypeRegistry::anyType());

  fundamentals->list = new Object(Node::NK_DICT, Location(), NULL);
  fundamentals->list->setName(fundamentals->str("list"));
  fundamentals->list->properties()[fundamentals->str("map")] =
      builder.createFunction(Location(),
          TypeRegistry::genericListType(), mapFunctionType, methodListMap);
}

}
