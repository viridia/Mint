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
  Node * actionArgs[] = { new IntegerLiteral(Node::NK_INTEGER, loc, TypeRegistry::integerType(), int(severity)), args };
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
  console->defineMethod("debug",  "u,msg:s", methodConsoleDebug);
  console->defineMethod("status", "u,msg:s", methodConsoleStatus);
  console->defineMethod("info",   "u,msg:s", methodConsoleInfo);
  console->defineMethod("warn",   "u,msg:s", methodConsoleWarn);
  console->defineMethod("error",  "u,msg:s", methodConsoleError);
  console->defineMethod("fatal",  "u,msg:s", methodConsoleFatal);

  /// Message methods -- deferred (build time) output to console
  Object * message = fundamentals->createChildScope("message");
  message->defineMethod("debug", "A,msg:s", methodMessageDebug);
  message->defineMethod("status","A,msg:s", methodMessageStatus);
  message->defineMethod("info",  "A,msg:s", methodMessageInfo);
  message->defineMethod("warn",  "A,msg:s", methodMessageWarn);
  message->defineMethod("error", "A,msg:s", methodMessageError);
  message->defineMethod("fatal", "A,msg:s", methodMessageFatal);
}

}
