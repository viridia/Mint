/* ================================================================== *
 * Console
 * ================================================================== */

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Object.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

using namespace mint::strings;

Node * methodConsoleDebug(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::DEBUG, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleStatus(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::STATUS, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleInfo(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::INFO, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleWarn(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::WARNING, loc, String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleError(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::ERROR, loc, String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleFatal(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::FATAL, loc, String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

void initConsoleMethods(Fundamentals * fundamentals) {
  Object * console = fundamentals->createChildScope("console");
  console->defineMethod(
      "debug", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleDebug);
  console->defineMethod(
      "status", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleStatus);
  console->defineMethod(
      "info", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleInfo);
  console->defineMethod(
      "warn", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleWarn);
  console->defineMethod(
      "error", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleError);
  console->defineMethod(
      "fatal", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleFatal);
}

}
