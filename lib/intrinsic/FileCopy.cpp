/* ================================================================== *
 * Fundamentals
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"

#include "mint/graph/Function.h"
#include "mint/graph/GraphBuilder.h"
#include "mint/graph/Module.h"
#include "mint/graph/Object.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"

namespace mint {

Node * methodCopyFile(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 3);
  String * source = String::cast(args[0]);
  String * output = String::cast(args[1]);
  Node * env = args[2];
  M_ASSERT(env != NULL);
  env->dump();

  // Calculate the module directory path.
  SmallString<64> moduleDir(path::parent(ex->module()->sourceDir()));
  diag::info() << moduleDir;
  (void)source;
  (void)output;

  M_ASSERT(false) << "Implement";
  return NULL;
}

void initFileCopyMethods(Fundamentals * fundamentals) {
  // Function 'glob'.
  GraphBuilder builder(fundamentals->typeRegistry());
  Type * argTypes[] = {
    TypeRegistry::stringType(),
    TypeRegistry::stringType(),
    fundamentals->object,
  };
  fundamentals->setProperty(
      fundamentals->str("copy_file"),
      builder.createFunction(
          Location(), TypeRegistry::undefinedType(), argTypes, methodCopyFile));
}

}
