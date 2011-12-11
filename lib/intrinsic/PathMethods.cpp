/* ================================================================== *
 * Path
 * ================================================================== */

#include "mint/eval/Evaluator.h"

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/String.h"

#include "mint/project/Project.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/Path.h"
#include "mint/support/OStream.h"

namespace mint {

static Module * currentModule(Evaluator * ex, Node * in) {
  for (Node * n = in; n != NULL; n = n->parentScope()) {
    if (n->nodeKind() == Node::NK_MODULE) {
      return static_cast<Module *>(n);
    }
  }
  for (Node * n = ex->lexicalScope(); n != NULL; n = n->parentScope()) {
    if (n->nodeKind() == Node::NK_MODULE) {
      return static_cast<Module *>(n);
    }
  }
  diag::error(in->location()) << "Could not find the current executing module.";
  return NULL;
}

static Module * topLevelModule(Evaluator * ex, Node * in) {
  Module * m = currentModule(ex, in);
  if (m != NULL) {
    Project * p = m->project();
    if (p == NULL) {
      diag::error(in->location()) << "Could not find top-level module.";
      return NULL;
    }
    return p->mainModule();
  }
  return NULL;
}

Node * methodPathAddExt(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * in = String::cast(args[0]);
  String * ext = String::cast(args[1]);
  SmallString<64> result(in->value());
  result.push_back('.');
  result.append(ext->value());
  return String::create(result);
}

Node * methodPathChangeExt(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * in = String::cast(args[0]);
  String * ext = String::cast(args[1]);
  SmallString<64> result(in->value());
  path::changeExtension(result, ext->value());
  return String::create(result);
}

Node * methodPathExt(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  return String::create(path::extension(String::cast(args[0])->value()));
}

Node * methodPathBasename(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * in = String::cast(args[0]);
  return String::create(path::filename(in->value()));
}

Node * methodPathDirname(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  String * in = String::cast(args[0]);
  return String::create(path::parent(in->value()));
}

Node * methodPathJoin(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 2);
  String * base = String::cast(args[0]);
  String * newpath = String::cast(args[1]);
  SmallString<64> result(base->value());
  path::combine(result, newpath->value());
  return String::create(result);
}

Node * methodPathTopLevelSourceDir(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 0);
  Module * m = topLevelModule(ex, self);
  return m ? String::create(m->sourceDir()) : &Node::UNDEFINED_NODE;
}

Node * methodPathTopLevelBuildDir(
    Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 0);
  Module * m = topLevelModule(ex, self);
  return m ? String::create(m->buildDir()) : &Node::UNDEFINED_NODE;
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
  path->defineMethod("top_level_source_dir", TypeRegistry::stringType(),
      methodPathTopLevelSourceDir);
  path->defineMethod("top_level_output_dir", TypeRegistry::stringType(), methodPathTopLevelBuildDir);
}

}
