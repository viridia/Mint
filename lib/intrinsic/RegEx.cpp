/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Object.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Path.h"
#include "mint/support/OStream.h"

namespace mint {

Node * methodRegExFind(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

Node * methodRegExSubst(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

Node * methodRegExSubstAll(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

Node * methodReCompile(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

void initRegExMethods(Fundamentals * fundamentals) {
  // Regular expression type
  fundamentals->regex = new Object(Node::NK_DICT, Location(), NULL);
  fundamentals->regex->setName(fundamentals->str("regex"));
  fundamentals->regex->defineMethod(
      "find", TypeRegistry::stringType(), TypeRegistry::stringType(), methodRegExFind);
  fundamentals->regex->defineMethod(
      "subst", TypeRegistry::stringType(), TypeRegistry::anyType(), methodRegExSubst);
  fundamentals->regex->defineMethod(
      "subst_all", TypeRegistry::stringType(), TypeRegistry::anyType(), methodRegExSubstAll);

  // Regular expression module
  Object * re = fundamentals->createChildScope("re");
  re->defineMethod("compile", fundamentals->regex, TypeRegistry::stringType(), methodReCompile);
}

}
