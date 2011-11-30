/* ================================================================== *
 * Mint
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Function.h"
#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"

namespace mint {

Node * methodFileRead(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * filename = String::cast(args[0]);
  SmallString<0> buffer;
  if (path::readFileContents(filename->value(), buffer)) {
    return String::create(buffer);
  }

  return &Node::UNDEFINED_NODE;
}

Node * methodFileCreate(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * filename = String::cast(args[0]);
  String * content = String::cast(args[1]);

  diag::info() << *filename;
  diag::info() << *content;

  M_ASSERT(false) << "Implement";
  return NULL;
}

void initFileMethods(Fundamentals * fundamentals) {
  Object * file = fundamentals->createChildScope("file");

  // Function 'glob'.
  Type * argTypes[] = {
    TypeRegistry::stringType(),
    TypeRegistry::stringType(),
    fundamentals->object,
  };
  file->defineMethod("read", TypeRegistry::undefinedType(), argTypes, methodFileRead);
}

}
