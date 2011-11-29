/* ================================================================== *
 * Path
 * ================================================================== */

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Object.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Path.h"
#include "mint/support/OStream.h"

namespace mint {

Node * methodPathAddExt(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * in = String::cast(args[0]);
  String * ext = String::cast(args[1]);
  SmallString<64> result(in->value());
  result.push_back('.');
  result.append(ext->value());
  return String::create(result);
}

Node * methodPathChangeExt(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * in = String::cast(args[0]);
  String * ext = String::cast(args[1]);
  SmallString<64> result(in->value());
  path::changeExtension(result, ext->value());
  return String::create(result);
}

Node * methodPathExt(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return String::create(path::extension(String::cast(args[0])->value()));
}

Node * methodPathBasename(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * in = String::cast(args[0]);
  return String::create(path::filename(in->value()));
}

Node * methodPathDirname(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * in = String::cast(args[0]);
  return String::create(path::parent(in->value()));
}

Node * methodPathJoin(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * base = String::cast(args[0]);
  String * newpath = String::cast(args[1]);
  SmallString<64> result(base->value());
  path::combine(result, newpath->value());
  return String::create(result);
}

void initPathMethods(Fundamentals * fundamentals) {
  Object * path = fundamentals->createChildScope("path");
  path->defineMethod(
      "add_ext", TypeRegistry::stringType(), TypeRegistry::stringType(), TypeRegistry::stringType(),
      methodPathAddExt);
  path->defineMethod(
      "change_ext", TypeRegistry::stringType(), TypeRegistry::stringType(), TypeRegistry::stringType(),
      methodPathChangeExt);
  path->defineMethod(
      "ext", TypeRegistry::stringType(), TypeRegistry::stringType(), methodPathExt);
  path->defineMethod(
      "basename", TypeRegistry::stringType(), TypeRegistry::stringType(), methodPathBasename);
  path->defineMethod(
      "dirname", TypeRegistry::stringType(), TypeRegistry::stringType(), methodPathDirname);
  // TODO: Make this a varargs function
  path->defineMethod(
      "join", TypeRegistry::stringType(), TypeRegistry::stringType(), TypeRegistry::stringType(),
      methodPathJoin);
}

}
