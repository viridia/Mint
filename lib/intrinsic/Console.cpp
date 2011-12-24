/* ================================================================== *
 * Console
 * ================================================================== */

#include "mint/intrinsic/Fundamentals.h"
#include "mint/intrinsic/StringRegistry.h"
#include "mint/intrinsic/TypeRegistry.h"

#include "mint/graph/Function.h"
#include "mint/graph/Literal.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/String.h"

#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/support/OStream.h"

namespace mint {

using namespace mint::strings;

static GCPointerRoot<Function> consoleDebug(NULL);
static GCPointerRoot<Function> consoleStatus(NULL);
static GCPointerRoot<Function> consoleInfo(NULL);
static GCPointerRoot<Function> consoleWarn(NULL);
static GCPointerRoot<Function> consoleError(NULL);
static GCPointerRoot<Function> consoleFatal(NULL);

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

Node * messageAction(Location loc, diag::Severity severity, Node * args) {
  Node * actionArgs[] = { new Literal<int>(Node::NK_INTEGER, loc, TypeRegistry::integerType(), int(severity)), args };
  return Oper::create(Node::NK_ACTION_MESSAGE, loc, TypeRegistry::actionType(), actionArgs);
}

Node * methodMessageDebug(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return messageAction(loc, diag::DEBUG, args[0]);
}

Node * methodMessageStatus(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return messageAction(loc, diag::STATUS, args[0]);
}

Node * methodMessageInfo(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return messageAction(loc, diag::INFO, args[0]);
}

Node * methodMessageWarn(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return messageAction(loc, diag::WARNING, args[0]);
}

Node * methodMessageError(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return messageAction(loc, diag::ERROR, args[0]);
}

Node * methodMessageFatal(Location loc, Evaluator * ex, Function * fn, Node * self, NodeArray args) {
  return messageAction(loc, diag::FATAL, args[0]);
}

void initConsoleMethods(Fundamentals * fundamentals) {
  /// Console methods -- immediate output to console
  Object * console = fundamentals->createChildScope("console");
  consoleDebug = console->defineMethod(
      "debug", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleDebug);
  consoleStatus = console->defineMethod(
      "status", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleStatus);
  consoleInfo = console->defineMethod(
      "info", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleInfo);
  consoleWarn = console->defineMethod(
      "warn", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleWarn);
  consoleError = console->defineMethod(
      "error", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleError);
  consoleFatal = console->defineMethod(
      "fatal", TypeRegistry::undefinedType(), TypeRegistry::stringType(), methodConsoleFatal);

  /// Message methods -- deferred (build time) output to console
  Object * message = fundamentals->createChildScope("message");
  message->defineMethod(
      "debug", TypeRegistry::actionType(), TypeRegistry::stringType(), methodMessageDebug);
  message->defineMethod(
      "status", TypeRegistry::actionType(), TypeRegistry::stringType(), methodMessageStatus);
  message->defineMethod(
      "info", TypeRegistry::actionType(), TypeRegistry::stringType(), methodMessageInfo);
  message->defineMethod(
      "warn", TypeRegistry::actionType(), TypeRegistry::stringType(), methodMessageWarn);
  message->defineMethod(
      "error", TypeRegistry::actionType(), TypeRegistry::stringType(), methodMessageError);
  message->defineMethod(
      "fatal", TypeRegistry::actionType(), TypeRegistry::stringType(), methodMessageFatal);
}

}
