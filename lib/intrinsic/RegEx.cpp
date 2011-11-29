/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/intrinsic/Fundamentals.h"

#include "mint/graph/GraphBuilder.h"
#include "mint/graph/Object.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Path.h"
#include "mint/support/OStream.h"

namespace mint {

Node * methodRegExFind(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

Node * methodRegExSubst(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

Node * methodRegExSubstAll(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

Node * methodReCompile(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return NULL;
}

void initRegExMethods(Fundamentals * fundamentals) {
  GraphBuilder builder(fundamentals->typeRegistry());

  // Regular expression type
  fundamentals->regex = new Object(Node::NK_DICT, Location(), NULL);
  fundamentals->regex->setName(fundamentals->str("regex"));
  fundamentals->regex->properties()[fundamentals->str("find")] =
      builder.createFunction(Location(),
          TypeRegistry::stringType(), TypeRegistry::stringType(), methodRegExFind);
  fundamentals->regex->properties()[fundamentals->str("subst")] =
      builder.createFunction(Location(),
          TypeRegistry::stringType(), TypeRegistry::anyType(), methodRegExSubst);
  fundamentals->regex->properties()[fundamentals->str("subst_all")] =
      builder.createFunction(Location(),
          TypeRegistry::stringType(), TypeRegistry::anyType(), methodRegExSubstAll);

  // Regular expression module
  String * strRe = fundamentals->str("re");
  Object * re = new Object(Node::NK_DICT, Location(), NULL);
  fundamentals->setProperty(strRe, re);
  re->setName(strRe);
  re->properties()[fundamentals->str("compile")] =
      builder.createFunction(Location(), fundamentals->regex, TypeRegistry::stringType(),
          methodReCompile);
}

}
