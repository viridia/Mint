/* ================================================================== *
 * Console
 * ================================================================== */

#include "mint/intrinsic/Fundamentals.h"

#include "mint/graph/GraphBuilder.h"
#include "mint/graph/Object.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

Node * methodConsoleDebug(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::DEBUG, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleStatus(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::STATUS, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleInfo(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::INFO, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleWarn(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::WARNING, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleError(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::ERROR, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

Node * methodConsoleFatal(Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  M_ASSERT(args.size() == 1);
  diag::writeMessage(diag::FATAL, Location(), String::cast(args[0])->value());
  return &Node::UNDEFINED_NODE;
}

void initConsoleMethods(Fundamentals * fundamentals) {
  GraphBuilder builder(fundamentals->typeRegistry());
  String * strConsole = fundamentals->str("console");
  Object * console = new Object(Node::NK_DICT, Location(), NULL);
  fundamentals->setProperty(strConsole, console);
  console->setName(strConsole);
  console->properties()[fundamentals->str("debug")] =
      builder.createFunction(Location(),
          TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleDebug);
  console->properties()[fundamentals->str("status")] =
      builder.createFunction(Location(),
          TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleStatus);
  console->properties()[fundamentals->str("info")] =
      builder.createFunction(Location(),
          TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleInfo);
  console->properties()[fundamentals->str("warn")] =
      builder.createFunction(Location(),
          TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleWarn);
  console->properties()[fundamentals->str("error")] =
      builder.createFunction(Location(),
          TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleError);
  console->properties()[fundamentals->str("fatal")] =
      builder.createFunction(Location(),
          TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleFatal);
}

}
