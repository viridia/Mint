/* ================================================================== *
 * Parser
 * ================================================================== */

#include "mint/parse/Parser.h"
#include "mint/support/Assert.h"
#include "mint/support/Diagnostics.h"
#include "mint/graph/Literal.h"
#include "mint/graph/Module.h"
#include "mint/graph/Object.h"
#include "mint/graph/Oper.h"
#include "mint/graph/String.h"
#include "mint/intrinsic/TypeRegistry.h"

#if HAVE_ERRNO_H
#include <errno.h>
#endif

#if HAVE_NEW
#include <new>
#endif

namespace mint {

namespace {

/// -------------------------------------------------------------------
/// Operator stack for operator precedence parsing.
class OperatorStack {
public:
  // Contains an operator/operand pair. The bottom element of the
  // stack contains only an operand:
  //
  //    [NULL value][op value][op value] ...
  struct Entry {
    Node * operand;
    Node::NodeKind operatorKind;
    unsigned predecence;
  };

  typedef SmallVector<Entry, 8> Stack;

  Stack stack;

  OperatorStack(Node * initialExpr) {
    stack.push_back(Entry());
    stack.back().operand = initialExpr;
  }
  bool pushOperand(Node * operand);
  bool pushOperator(Node::NodeKind operatorKind, unsigned predecence);
  bool reduce(unsigned precedence);
  bool reduceAll();

  Node * getExpression() const {
    return stack.front().operand;
  }
};

bool OperatorStack::pushOperand(Node * operand) {
  M_ASSERT(stack.back().operand == NULL);
  stack.back().operand = operand;
  return true;
}

bool OperatorStack::pushOperator(Node::NodeKind operatorKind, unsigned predecence) {
  M_ASSERT(stack.back().operand != NULL);
  if (!reduce(predecence)) {
    return false;
  }

  stack.push_back(Entry());
  stack.back().operatorKind = operatorKind;
  stack.back().predecence = predecence;
  stack.back().operand = NULL;
  return true;
}

bool OperatorStack::reduce(unsigned predecence) {
  while (stack.size() > 1) {
    Entry & back = stack.back();
    if (back.predecence < predecence) {
      break;
    }
    Node * val = back.operand;
    Node::NodeKind operatorKind = back.operatorKind;
    M_ASSERT(val != NULL);
    stack.pop_back();
    Location loc = stack.back().operand->location() | val->location();
    Node * args[2] = { stack.back().operand, val };
    Oper * op = Oper::create(operatorKind, loc, NULL, args);
    stack.back().operand = op;
  }
  return true;
}

bool OperatorStack::reduceAll() {
  if (!reduce(0))
    return false;
  M_ASSERT(stack.size() == 1);
  return true;
}

}

// Operator precedence levels.
enum Precedence {
  PREC_LOWEST = 0,
  PREC_MAPSTO = 5,
  PREC_LOGICALOR = 5,
  PREC_LOGICALAND = 6,
  PREC_CONTAINS = 7,
  PREC_ISTYPE = 8,

  PREC_RELATIONAL = 10,

  PREC_BITOR = 20,
  PREC_BITXOR = 21,
  PREC_BITAND = 22,

  PREC_SHIFT = 25,
  PREC_ADDSUB = 30,
  PREC_MULDIV = 32,

  PREC_EXPONENT = 40,
  PREC_RANGE = 50,

  //PREC_ATTR = 60,

  PREC_HIGHEST
};

Parser::Parser(TextBuffer * src)
  : _lexer(src)
  , _recover(false)
{
  _token = _lexer.next();
}

void Parser::next() {
  _tokenLoc = _lexer.tokenLocation();
  _token = _lexer.next();
}

bool Parser::match(Token tok) {
  if (_token == tok) {
    next();
    return true;
  }
  return false;
}

String * Parser::matchIdent() {
  if (_token == TOKEN_IDENT) {
    // Save the token value as a string
    String * value = String::create(
        Node::NK_IDENT,
        _lexer.tokenLocation(),
        TypeRegistry::stringType(),
        _lexer.tokenValue());

    // Get the next token
    next();
    return value;
  }
  return NULL;
}

Oper * Parser::parseModule() {
  NodeList args;
  if (!definitionList(args)) {
    return NULL;
  }
  return Oper::create(Node::NK_MAKE_MODULE, Location(), NULL, args);
}

bool Parser::parseProjects(SmallVectorImpl<Node *> & projects) {
  while (diag::errorCount() == 0) {
    switch (_token) {
      case TOKEN_END:
      case TOKEN_ERROR:
        return true;

      case TOKEN_PROJECT: {
        next();
        Location loc = _lexer.tokenLocation();
        if (_token != TOKEN_STRING) {
          expected("project directory");
          break;
        }

        Node * location = parseStringLiteral();
        if (!match(TOKEN_LBRACE)) {
          expected("{");
          break;
        }
        NodeList args;
        args.push_back(location);
        if (!definitionList(args)) {
          break;
        }
        projects.push_back(Oper::create(Node::NK_PROJECT, loc, NULL, args));
        if (!match(TOKEN_RBRACE)) {
          expected("}");
          break;
        }
        break;
      }

      default:
        expected("project definition");
        return false;
    }
  }
  return false;
}

Node * Parser::definitionList() {
  NodeList args;
  if (!definitionList(args)) {
    return NULL;
  }
  return Oper::create(Node::NK_LIST, Location(), NULL, args);
}

bool Parser::definitionList(NodeList & results) {
  while (diag::errorCount() == 0) {
    switch (_token) {
      case TOKEN_END:
      case TOKEN_ERROR:
      case TOKEN_RBRACE:
      case TOKEN_ELSE:
        return true;

      case TOKEN_IMPORT: {
        Location loc(_lexer.tokenLocation());
        next();
        Node * name = importName();
        if (name == NULL) {
          skipToEndOfLine();
          return NULL;
        }
        loc |= name->location();
        Node * asName = NULL;
        if (match(TOKEN_AS)) {
          asName = matchIdent();
          if (asName == NULL) {
            expected("import name");
            skipToEndOfLine();
            return NULL;
          }
          Node * impArgs[] = { name, asName };
          results.push_back(Oper::create(Node::NK_IMPORT_AS, loc | asName->location(), NULL, impArgs));
        } else {
          Node * impArgs[] = { name };
          results.push_back(Oper::create(Node::NK_IMPORT, loc, NULL, impArgs));
        }
        break;
      }

      case TOKEN_FROM: {
        Location loc(_lexer.tokenLocation());
        next();
        Node * name = importName();
        if (name == NULL) {
          skipToEndOfLine();
          return NULL;
        }
        if (!match(TOKEN_IMPORT)) {
          expected("'import' keyword");
          skipToEndOfLine();
          return NULL;
        }

        NodeList impArgs;
        impArgs.push_back(name);
        if (match(TOKEN_STAR)) {
          loc |= _lexer.tokenLocation();
          results.push_back(Oper::create(Node::NK_IMPORT_ALL, loc, NULL, impArgs));
        } else {
          Node * sym = matchIdent();
          if (sym == NULL) {
            expected("identifier after 'import'");
            skipToEndOfLine();
            return NULL;
          }
          impArgs.push_back(sym);
          loc |= sym->location();
          while (match(TOKEN_COMMA)) {
            sym = matchIdent();
            if (sym == NULL) {
              expected("identifier after ','");
              skipToEndOfLine();
              return NULL;
            }
            impArgs.push_back(sym);
            loc |= sym->location();
          }
          results.push_back(Oper::create(Node::NK_IMPORT_FROM, loc, NULL, impArgs));
        }
        break;
      }

      case TOKEN_DO: {
        next();
        Node * action = expression();
        if (action == NULL) {
          skipToEndOfLine();
          continue;
        }
        results.push_back(
            Oper::create(Node::NK_MAKE_ACTION, action->location(), NULL, makeArrayRef(action)));
        break;
      }

      case TOKEN_IF: {
        next();
        Node * n = ifDirective();
        if (n == NULL) {
          skipToEndOfLine();
          continue;
        }
        results.push_back(n);
        break;
      }

      case TOKEN_IDENT: {
        Node * attrName = primaryExpression();
        if (attrName == NULL) {
          skipToEndOfLine();
          continue;
        }
        if (!match(TOKEN_ASSIGN)) {
          expected("assignment");
          skipToEndOfLine();
          continue;
        }
        Node * attrValue = expression();
        if (attrValue == NULL) {
          skipToEndOfLine();
          continue;
        }

        Node * setAttrArgs[] = { attrName, attrValue };
        results.push_back(
            Oper::create(Node::NK_SET_MEMBER,
                attrName->location() | attrValue->location(), NULL, setAttrArgs));
        break;
      }

      default:
        expected("definition");
        return false;
    }
  }
  return false;
}

Node * Parser::importName() {
  Location loc = _lexer.tokenLocation();
  String * name = matchIdent();
  if (name == NULL) {
    expected("import name");
    return NULL;
  }

  // Project name qualifier.
  SmallString<64> path(name->value());
  if (match(TOKEN_COLON)) {
    name = matchIdent();
    if (name == NULL) {
      expectedIdentifier();
      return NULL;
    }

    loc |= name->location();
    path.push_back(':');
    path.append(name->value());
  }

  while (match(TOKEN_DOT)) {
    // Member dereference
    name = matchIdent();
    if (name == NULL) {
      expectedIdentifier();
      break;
    }

    loc |= name->location();
    path.push_back('.');
    path.append(name->value());
  }

  return String::create(Node::NK_STRING, loc, TypeRegistry::stringType(), path);
}

Node * Parser::ifDirective() {
  Location loc = _lexer.tokenLocation();
  if (!match(TOKEN_LPAREN)) {
    expected("(");
    return NULL;
  }
  Node * test = expression();
  if (test == NULL) {
    return NULL;
  }
  if (!match(TOKEN_RPAREN)) {
    expectedCloseParen();
    return NULL;
  }
  Node * thenBody = definitionList();
  if (thenBody == NULL) {
    return NULL;
  }
  if (match(TOKEN_ELSE)) {
    Node * elseBody = definitionList();
    if (elseBody == NULL) {
      return NULL;
    }
    Node * args[] = { test, thenBody, elseBody };
    return Oper::create(Node::NK_IF, loc, NULL, args);
  } else {
    Node * args[] = { test, thenBody };
    return Oper::create(Node::NK_IF, loc, NULL, args);
  }
}

Node * Parser::expression() {
  return binaryOperator();
}

Node * Parser::binaryOperator() {
  Node * e0 = unaryOperator();
  if (e0 == NULL) {
    return NULL;
  }

  OperatorStack opstack(e0);
  for (;;) {
    Token operatorToken = _token;
    Location loc = _lexer.tokenLocation();

    switch (_token) {
      case TOKEN_PLUS:
        opstack.pushOperator(Node::NK_ADD, PREC_ADDSUB);
        next();
        break;

      case TOKEN_MINUS:
        opstack.pushOperator(Node::NK_SUBTRACT, PREC_ADDSUB);
        next();
        break;

      case TOKEN_STAR:
        opstack.pushOperator(Node::NK_MULTIPLY, PREC_MULDIV);
        next();
        break;

      case TOKEN_SLASH:
        opstack.pushOperator(Node::NK_DIVIDE, PREC_MULDIV);
        next();
        break;

      case TOKEN_PERCENT:
        opstack.pushOperator(Node::NK_MODULUS, PREC_MULDIV);
        next();
        break;

      case TOKEN_DOUBLE_PLUS:
        opstack.pushOperator(Node::NK_CONCAT, PREC_ADDSUB);
        next();
        break;

      case TOKEN_AND:
        opstack.pushOperator(Node::NK_AND, PREC_LOGICALAND);
        next();
        break;

      case TOKEN_OR:
        opstack.pushOperator(Node::NK_OR, PREC_LOGICALOR);
        next();
        break;

      case TOKEN_EQUAL:
        opstack.pushOperator(Node::NK_EQUAL, PREC_RELATIONAL);
        next();
        break;

      case TOKEN_NOTEQUAL:
        opstack.pushOperator(Node::NK_NOT_EQUAL, PREC_RELATIONAL);
        next();
        break;

      case TOKEN_LESS:
        opstack.pushOperator(Node::NK_LESS, PREC_RELATIONAL);
        next();
        break;

      case TOKEN_GREATER:
        opstack.pushOperator(Node::NK_GREATER, PREC_RELATIONAL);
        next();
        break;

      case TOKEN_LESS_EQUAL:
        opstack.pushOperator(Node::NK_LESS_EQUAL, PREC_RELATIONAL);
        next();
        break;

      case TOKEN_GREATER_EQUAL:
        opstack.pushOperator(Node::NK_GREATER_EQUAL, PREC_RELATIONAL);
        next();
        break;

      case TOKEN_MAPS_TO:
        opstack.pushOperator(Node::NK_MAPS_TO, PREC_MAPSTO);
        next();
        break;

      case TOKEN_IN:
        opstack.pushOperator(Node::NK_IN, PREC_CONTAINS);
        next();
        break;

      case TOKEN_NOT: {
        next();
        Location loc = loc;
        if (match(TOKEN_IN)) {
          opstack.pushOperator(Node::NK_NOT_IN, PREC_CONTAINS);
        } else {
          diag::error(loc) << "'in' expected after 'not'";
        }
        break;
      }

      default:
        goto done;
    }

    Node * e1 = unaryOperator();
    if (e1 == NULL) {
      diag::error(_lexer.tokenLocation()) << "value expected after " << getTokenName(operatorToken);
      return NULL;
    }
    opstack.pushOperand(e1);
  }

done:
  if (!opstack.reduceAll()) {
    return e0;
  }

  return opstack.getExpression();
}

Node * Parser::unaryOperator() {
  switch (_token) {
    case TOKEN_NOT: {
      // Negated operators
      next();
      Location loc = _lexer.tokenLocation();
      Node * e1 = unaryOperator();
      if (e1 == NULL) {
        return NULL;
      }
      return Oper::create(Node::NK_NOT, loc, e1->type(), makeArrayRef(e1));
    }

//    case Token_Tilde: {
//      // Negated operators
//      next();
//      Location loc = lexer.tokenLocation();
//      ASTNode * e1 = unaryOperator();
//      if (e1 == NULL)
//        return NULL;
//      ASTOper * result = new ASTOper(ASTNode::Complement, loc);
//      result->append(e1);
//      return result;
//    }

    case TOKEN_MINUS: {
      // Negated operators
      next();
      Location loc = _lexer.tokenLocation();
      Node * e1 = unaryOperator();
      if (e1 == NULL) {
        return NULL;
      }
      return Oper::create(Node::NK_NEGATE, loc, e1->type(), makeArrayRef(e1));
    }

    default:
      return primaryExpression();
  }
}

Node * Parser::primaryExpression() {
  Node * result = NULL;

  Location loc = _lexer.tokenLocation();
  switch (int(_token)) {
    case TOKEN_LPAREN:
      next();
      result = expression();
      if (match(TOKEN_COMMA)) { // A tuple
        NodeList args;
        args.push_back(result);
        while (_token != TOKEN_END && _token != TOKEN_ERROR && !match(TOKEN_RPAREN)) {
          Node * n = expression();
          if (n == NULL) {
            skipToCloseDelim(TOKEN_COMMA, TOKEN_RPAREN);
            continue;
          }
          args.push_back(n);
          if (_token != TOKEN_RPAREN && !match(TOKEN_COMMA)) {
            expectedCloseParen();
            skipToCloseDelim(TOKEN_END, TOKEN_RPAREN);
          }
        }
        loc |= _tokenLoc;
        result = Oper::create(Node::NK_MAKE_TUPLE, loc, NULL, args);
      } else if (!match(TOKEN_RPAREN)) {
        expectedCloseParen();
        return NULL;
      }
      break;

    case TOKEN_LBRACE:
      next();
      result = parseDictionaryLiteral();
      break;

    case TOKEN_INTEGER:
      result = parseIntegerLiteral();
      break;

    case TOKEN_FLOAT:
      result = parseFloatLiteral();
      break;

    case TOKEN_IDENT: {
      result = matchIdent();
      break;
    }

    case TOKEN_SELF: {
      next();
      result = new Node(Node::NK_SELF, loc, NULL);
      break;
    }

    case TOKEN_SUPER: {
      next();
      result = new Node(Node::NK_SUPER, loc, NULL);
      break;
    }

    case TOKEN_STRING:
      result = parseStringLiteral();
      break;

    case TOKEN_ISTRING_START:
      result = parseInterpolatedStringLiteral();
      break;

    case TOKEN_TRUE: {
      next();
      return new Literal<bool>(Node::NK_BOOL, loc, TypeRegistry::boolType(), true);
    }

    case TOKEN_FALSE: {
      next();
      return new Literal<bool>(Node::NK_BOOL, loc, TypeRegistry::boolType(), false);
    }

    case TOKEN_UNDEFINED: {
      next();
      return new Node(Node::NK_UNDEFINED, loc, TypeRegistry::undefinedType());
    }

    case TOKEN_LBRACKET:
      next();
      result = parseListLiteral();
      break;

    case TOKEN_DO:
      next();
      result = doStmt();
      break;

    case TOKEN_LET:
      next();
      result = letStmt();
      break;

    case TOKEN_IF:
      next();
      result = ifStmt();
      break;

    case TOKEN_TYPENAME_ANY: {
      next();
      result = TypeRegistry::anyType();
      break;
    }

    case TOKEN_TYPENAME_BOOL: {
      next();
      result = TypeRegistry::boolType();
      break;
    }

    case TOKEN_TYPENAME_INT: {
      next();
      result = TypeRegistry::integerType();
      break;
    }

    case TOKEN_TYPENAME_STRING: {
      next();
      result = TypeRegistry::stringType();
      break;
    }

    case TOKEN_TYPENAME_FLOAT: {
      next();
      result = TypeRegistry::floatType();
      break;
    }

    case TOKEN_TYPENAME_LIST: {
      next();
      result = TypeRegistry::genericListType();
      break;
    }

    case TOKEN_TYPENAME_DICT: {
      next();
      result = TypeRegistry::genericDictType();
      break;
    }

    default:
      diag::error(loc) << "Invalid token: " << getTokenName(_token);
      break;
  }

  // Suffix operators
  if (result) {
    for (;;) {
      bool lineBreakBefore = _lexer.lineBreakBefore();
      loc = _lexer.tokenLocation();
      if (!lineBreakBefore && match(TOKEN_LPAREN)) {
        // Call
        NodeList args;
        args.push_back(result);
        loc = result->location();
        if (!parseArgumentList(args, loc)) {
          return NULL;
        }
        result = Oper::create(Node::NK_CALL, loc, NULL, args);
      } else if (!lineBreakBefore && match(TOKEN_LBRACKET)) {
        // Array dereference
        NodeList args;
        args.push_back(result);
        while (!match(TOKEN_RBRACKET)) {
          Node * arg = expression();
          if (arg == NULL) {
            return NULL;
          }
          args.push_back(arg);
          if (_token != TOKEN_RBRACKET && !match(TOKEN_COMMA)) {
            expectedCloseBracket();
            skipToCloseDelim(TOKEN_END, TOKEN_RBRACKET);
          }
          loc |= _lexer.tokenLocation();
        }
        result = Oper::create(Node::NK_GET_ELEMENT, loc | result->location(), NULL, args);
      } else if (match(TOKEN_DOT)) {
        // Member dereference
        String * ident = matchIdent();
        if (ident == NULL) {
          expectedIdentifier();
          break;
        }

        loc = result->location() | ident->location();
        Node * args[2] = { result, ident };
        result = Oper::create(Node::NK_GET_MEMBER, loc, NULL, args);
      } else if (result && match(TOKEN_LBRACE)) {
        result = parseObjectLiteral(result);
      } else {
        break;
      }
    }
  }

  return result;
}

Node * Parser::primaryTypeExpression() {
  Node * result = NULL;

  Location loc = _lexer.tokenLocation();
  switch (int(_token)) {
    case TOKEN_LPAREN:
      next();
      result = expression();
      if (match(TOKEN_COMMA)) { // A tuple
        NodeList args;
        args.push_back(result);
        while (_token != TOKEN_END && _token != TOKEN_ERROR && !match(TOKEN_RPAREN)) {
          Node * n = expression();
          if (n == NULL) {
            skipToCloseDelim(TOKEN_COMMA, TOKEN_RPAREN);
            continue;
          }
          args.push_back(n);
          if (_token != TOKEN_RPAREN && !match(TOKEN_COMMA)) {
            expectedCloseParen();
            skipToCloseDelim(TOKEN_END, TOKEN_RPAREN);
          }
        }
        loc |= _tokenLoc;
        result = Oper::create(Node::NK_MAKE_TUPLE, loc, NULL, args);
      } else if (!match(TOKEN_RPAREN)) {
        expectedCloseParen();
        return NULL;
      }
      break;

    case TOKEN_IDENT: {
      result = matchIdent();
      break;
    }

    case TOKEN_UNDEFINED: {
      next();
      return new Node(Node::NK_UNDEFINED, loc, TypeRegistry::undefinedType());
    }

    case TOKEN_LBRACKET:
      next();
      result = parseListLiteral();
      break;

    case TOKEN_TYPENAME_ANY: {
      next();
      result = TypeRegistry::anyType();
      break;
    }

    case TOKEN_TYPENAME_BOOL: {
      next();
      result = TypeRegistry::boolType();
      break;
    }

    case TOKEN_TYPENAME_INT: {
      next();
      result = TypeRegistry::integerType();
      break;
    }

    case TOKEN_TYPENAME_STRING: {
      next();
      result = TypeRegistry::stringType();
      break;
    }

    case TOKEN_TYPENAME_FLOAT: {
      next();
      result = TypeRegistry::floatType();
      break;
    }

    case TOKEN_TYPENAME_LIST: {
      next();
      result = TypeRegistry::genericListType();
      break;
    }

    case TOKEN_TYPENAME_DICT: {
      next();
      result = TypeRegistry::genericDictType();
      break;
    }

    default:
      diag::error(loc) << "Invalid token: " << getTokenName(_token);
      break;
  }

  // Suffix operators
  if (result) {
    for (;;) {
      bool lineBreakBefore = _lexer.lineBreakBefore();
      loc = _lexer.tokenLocation();
      if (!lineBreakBefore && match(TOKEN_LBRACKET)) {
        // Array dereference
        NodeList args;
        args.push_back(result);
        while (!match(TOKEN_RBRACKET)) {
          Node * arg = expression();
          if (arg == NULL) {
            return NULL;
          }
          args.push_back(arg);
          if (_token != TOKEN_RBRACKET && !match(TOKEN_COMMA)) {
            expectedCloseBracket();
            skipToCloseDelim(TOKEN_END, TOKEN_RBRACKET);
          }
          loc |= _lexer.tokenLocation();
        }
        result = Oper::create(Node::NK_GET_ELEMENT, loc | result->location(), NULL, args);
      } else if (match(TOKEN_DOT)) {
        // Member dereference
        String * ident = matchIdent();
        if (ident == NULL) {
          expectedIdentifier();
          break;
        }

        loc = result->location() | ident->location();
        Node * args[2] = { result, ident };
        result = Oper::create(Node::NK_GET_MEMBER, loc, NULL, args);
      } else {
        break;
      }
    }
  }

  return result;
}

Node * Parser::doStmt() {
  if (!match(TOKEN_LBRACKET)) {
    expected("]");
    return NULL;
  }
  NodeList args;
  Location loc = _tokenLoc;
  while (_token != TOKEN_END && _token != TOKEN_ERROR && !match(TOKEN_RBRACKET)) {
    Node * n = expression();
    if (n == NULL) {
      skipToCloseDelim(TOKEN_COMMA, TOKEN_RBRACKET);
      continue;
    }
    args.push_back(n);
    if (_token != TOKEN_RBRACKET && !match(TOKEN_COMMA)) {
      expectedCloseBracket();
      skipToCloseDelim(TOKEN_END, TOKEN_RBRACKET);
    }
  }
  loc |= _tokenLoc;
  return Oper::create(Node::NK_DO, loc, NULL, args);
}

Node * Parser::letStmt() {
  NodeList args;
  Location loc = _lexer.tokenLocation();
  while (_token != TOKEN_END && _token != TOKEN_ERROR) {
    Node * attrName = matchIdent();
    if (attrName == NULL) {
      expectedIdentifier();
      return NULL;
    }
    if (!match(TOKEN_ASSIGN)) {
      expected("=");
      return NULL;
    }
    Node * attrValue = expression();
    if (attrValue == NULL) {
      return NULL;
    }
    Node * setAttrArgs[] = { attrName, attrValue };
    args.push_back(
        Oper::create(Node::NK_SET_MEMBER,
            attrName->location() | attrValue->location(), NULL, setAttrArgs));
    if (match(TOKEN_COMMA)) {
      continue;
    } else if (match(TOKEN_COLON)) {
      break;
    } else {
      expected(":");
    }
  }
  Node * body;
  if (_token == TOKEN_LBRACKET) {
    body = doStmt();
  } else {
    body = expression();
  }
  if (body == NULL) {
    return NULL;
  }
  loc |= body->location();
  args.push_back(body);
  return Oper::create(Node::NK_LET, loc, NULL, args);
}

Node * Parser::ifStmt() {
  Location loc = _lexer.tokenLocation();
  if (!match(TOKEN_LPAREN)) {
    expected("(");
    return NULL;
  }
  Node * test = expression();
  if (test == NULL) {
    return NULL;
  }
  if (!match(TOKEN_RPAREN)) {
    expectedCloseParen();
    return NULL;
  }
  Node * thenBody = expression();
  if (thenBody == NULL) {
    return NULL;
  }
  if (!match(TOKEN_ELSE)) {
    expected("else");
    return NULL;
  }
  Node * elseBody = expression();
  if (elseBody == NULL) {
    return NULL;
  }

  Node * args[] = { test, thenBody, elseBody };
  return Oper::create(Node::NK_IF, loc, NULL, args);
}

bool Parser::parseArgumentList(NodeList & args, Location & l) {
  if (match(TOKEN_RPAREN)) {
    return true;
  }

  for (;;) {
    Node * n = expression();
    if (n == NULL) {
      //expectedCloseParen();
      //skipToRParen();
      return false;
    }
    args.push_back(n);

    l |= _lexer.tokenLocation();
    if (match(TOKEN_RPAREN)) {
      return true;
    } else if (!match(TOKEN_COMMA)) {
      expectedCloseParen();
    }
  }
  return false;
}

Node * Parser::parseObjectLiteral(Node * prototype) {
  Location loc = _tokenLoc;
  NodeList args;
  args.push_back(prototype);
  while (!match(TOKEN_RBRACE)) {
    unsigned attrFlags = 0;
    if (match(TOKEN_CACHED)) {
      attrFlags |= AttributeDefinition::CACHED;
    }
    if (match(TOKEN_PARAM)) {
      attrFlags |= AttributeDefinition::PARAM;
      Node * param = parseObjectParam(attrFlags);
      if (param == NULL) {
        skipToEndOfLine();
        continue;
      }
      args.push_back(param);
    } else if (_token == TOKEN_IDENT) {
      if (attrFlags != 0) {
        expected("parameter definition after 'cached' modifier");
        skipToEndOfLine();
        continue;
      }
      Node * attrName = primaryExpression();
      if (attrName == NULL) {
        skipToEndOfLine();
        continue;
      }
      bool deferred = false;
      Node::NodeKind opKind = Node::NK_SET_MEMBER;
      if (match(TOKEN_MAPS_TO)) {
        deferred = true;
      } else if (match(TOKEN_DOUBLE_PLUS_ASSIGN)) {
        opKind = Node::NK_APPEND_MEMBER;
      } else if (match(TOKEN_ASSIGN)) {
        opKind = Node::NK_SET_MEMBER;
      } else {
        expected("assignment");
      }
      Node * attrValue = expression();
      if (attrValue == NULL) {
        skipToEndOfLine();
        continue;
      }

      if (deferred) {
        Node * deferredArgs[] = { attrValue };
        attrValue = Oper::create(Node::NK_MAKE_DEFERRED, attrValue->location(), NULL, deferredArgs);
      }

      Node * setAttrArgs[] = { attrName, attrValue };
      args.push_back(
          Oper::create(opKind,
              attrName->location() | attrValue->location(), NULL, setAttrArgs));
    } else if (_token == TOKEN_ERROR) {
      lexerError();
      skipToEndOfLine();
    } else {
      diag::error(_lexer.tokenLocation()) << "Expected object attribute definition, was "
          << getTokenName(_token);
      skipToEndOfLine();
    }

    if (match(TOKEN_COMMA) || _lexer.lineBreakBefore()) {
      continue;
    } else if (_token != TOKEN_RBRACE) {
      expected("comma or '}");
      skipToEndOfLine();
    }
  }
  loc |= _tokenLoc;
  return Oper::create(Node::NK_MAKE_OBJECT, loc, NULL, args);
}

Node * Parser::parseObjectParam(unsigned flags) {
  Location loc = _tokenLoc;
  Node * name = matchIdent();
  Node * type = NULL;
  if (name == NULL) {
    expected("parameter name");
    return NULL;
  }
  if (match(TOKEN_COLON)) {
    type = primaryTypeExpression();
  }
  bool deferred = false;
  Node * attrValue = NULL;
  if (match(TOKEN_MAPS_TO)) {
    deferred = true;
    attrValue = expression();
  } else if (match(TOKEN_ASSIGN)) {
    attrValue = expression();
  } else if (_lexer.lineBreakBefore()) {
    attrValue = &Node::UNDEFINED_NODE;
  } else {
    expected("assignment");
    return NULL;
  }
  if (attrValue == NULL) {
    skipToEndOfLine();
    return NULL;
  }

  if (deferred) {
    Node * deferredArgs[] = { attrValue };
    attrValue = Oper::create(Node::NK_MAKE_DEFERRED, attrValue->location(), NULL, deferredArgs);
  }

  loc |= _tokenLoc;
  Node * attrFlags = new Literal<int>(Node::NK_INTEGER, Location(), NULL, flags);
  Node * setAttrArgs[] = { name, type, attrValue, attrFlags };
  return Oper::create(Node::NK_MAKE_PARAM, loc, NULL, setAttrArgs);
}

Node * Parser::parseListLiteral() {
  NodeList args;
  Location loc = _tokenLoc;
  while (_token != TOKEN_END && _token != TOKEN_ERROR && !match(TOKEN_RBRACKET)) {
    Node * n = expression();
    if (n == NULL) {
      skipToCloseDelim(TOKEN_COMMA, TOKEN_RBRACKET);
      continue;
    }
    args.push_back(n);
    if (_token != TOKEN_RBRACKET && !match(TOKEN_COMMA) && !_lexer.lineBreakBefore()) {
      expectedCloseBracket();
      skipToCloseDelim(TOKEN_END, TOKEN_RBRACKET);
    }
  }
  loc |= _tokenLoc;
  return Oper::create(Node::NK_MAKE_LIST, loc, NULL, args);
}

Node * Parser::parseDictionaryLiteral() {
  NodeList args;
  Location loc = _tokenLoc;
  while (_token != TOKEN_END && _token != TOKEN_ERROR && !match(TOKEN_RBRACE)) {
    Node * key = primaryExpression();
    if (key == NULL) {
      skipToCloseDelim(TOKEN_COMMA, TOKEN_RBRACE);
      continue;
    }
    Node::NodeKind opKind = Node::NK_SET_MEMBER;
    if (match(TOKEN_DOUBLE_PLUS_ASSIGN)) {
      opKind = Node::NK_APPEND_MEMBER;
    } else if (!match(TOKEN_ASSIGN)) {
      expected("=");
      skipToCloseDelim(TOKEN_COMMA, TOKEN_RBRACE);
      continue;
    }
    Node * value = expression();
    if (value == NULL) {
      skipToCloseDelim(TOKEN_COMMA, TOKEN_RBRACE);
      continue;
    }

    Node * setAttrArgs[] = { key, value };
    args.push_back(
        Oper::create(opKind,
            key->location() | value->location(), NULL, setAttrArgs));

    if (_token != TOKEN_RBRACE && !match(TOKEN_COMMA)) {
      expectedCloseBracket();
      skipToCloseDelim(TOKEN_END, TOKEN_RBRACE);
    }
  }
  loc |= _tokenLoc;
  return Oper::create(Node::NK_MAKE_DICT, loc, NULL, args);
}

Node * Parser::parseIntegerLiteral() {
  int numberBase = 10;

  StringRef tokenVal = _lexer.tokenValueCStr();
  Location loc = _lexer.tokenLocation();

  // Check for hex number
  if (tokenVal.size() >= 2 && tokenVal[0] == '0' &&
      (tokenVal[1] == 'x' || tokenVal[1] == 'X')) {
    tokenVal = tokenVal.substr(2);
    numberBase = 16;
  }

  char * end;
  long value = ::strtol(tokenVal.data(), &end, numberBase);
  if (value == 0 && end == tokenVal.data() && errno == ERANGE) {
    diag::error(loc) << "Integer value '" << tokenVal << "' out of range.";
  }

  next();
  return new Literal<long>(Node::NK_INTEGER, loc, TypeRegistry::integerType(), value);
}

Node * Parser::parseFloatLiteral() {
  StringRef tokenVal = _lexer.tokenValueCStr();
  Location loc = _lexer.tokenLocation();

  char * end = NULL;
  double value = ::strtod(tokenVal.data(), &end);
  if (value == 0 && end == tokenVal.data() && errno == ERANGE) {
    diag::error(loc) << "Floating point value '" << tokenVal << "' out of range.";
  }

  next();
  return new Literal<double>(Node::NK_FLOAT, loc, &TypeRegistry::FLOAT_TYPE, value);
}

Node * Parser::parseStringLiteral() {
  Location loc = _lexer.tokenLocation();
  StringRef strValue = _lexer.tokenValue();
  Node * result = String::create(loc, strValue);
  next();
  return result;
}

Node * Parser::parseInterpolatedStringLiteral() {
  NodeList args;
  Location loc = _lexer.tokenLocation();
  next();
  while (!match(TOKEN_ISTRING_END)) {
    if (_token == TOKEN_STRING) {
      args.push_back(String::create(_lexer.tokenLocation(), _lexer.tokenValue()));
      next();
    } else if (_token == TOKEN_IDENT) {
      loc = _lexer.tokenLocation();
      Node * result = matchIdent();
      for (;;) {
        if (match(TOKEN_LBRACKET)) {
          // Array dereference
          NodeList args;
          args.push_back(result);
          while (!match(TOKEN_RBRACKET)) {
            Node * arg = expression();
            if (arg == NULL) {
              return NULL;
            }
            args.push_back(arg);
            if (_token != TOKEN_RBRACKET && !match(TOKEN_COMMA)) {
              expectedCloseBracket();
              skipToCloseDelim(TOKEN_END, TOKEN_RBRACKET);
            }
            loc |= _lexer.tokenLocation();
          }
          result = Oper::create(Node::NK_GET_ELEMENT, loc | result->location(), NULL, args);
        } else if (match(TOKEN_DOT)) {
          // Member dereference
          String * ident = matchIdent();
          if (ident == NULL) {
            expectedIdentifier();
            break;
          }

          loc = result->location() | ident->location();
          Node * args[2] = { result, ident };
          result = Oper::create(Node::NK_GET_MEMBER, loc, NULL, args);
        } else {
          break;
        }
      }
      args.push_back(result);
    } else {
      break;
    }
  }

  if (args.size() == 0) {
    return String::create(loc, "");
  } else if (args.size() == 1) {
    return args[0];
  } else {
    return Oper::create(Node::NK_CONCAT, loc, TypeRegistry::stringType(), args);
  }
}

void Parser::skipToNextOpenDelim() {
  for (;;) {
    if (_token == TOKEN_END || _token == TOKEN_LPAREN || _token == TOKEN_LBRACE ||
        _token == TOKEN_LBRACKET || _token == TOKEN_ERROR) {
      return;
    }

    next();
  }
}

void Parser::skipToEndOfLine() {
  while (!_lexer.lineBreakBefore()) {
    if (_token == TOKEN_END || _token == TOKEN_ERROR) {
      return;
    } else if (match(TOKEN_LPAREN)) {
      skipToCloseParen();
    } else if (match(TOKEN_LBRACE)) {
    } else if (match(TOKEN_LBRACKET)) {
    }

    next();
  }
}

void Parser::skipToCloseDelim(Token stopToken, Token endDelim) {
  for (;;) {
    if (_token == stopToken) {
      next();
      return;
    } else if (_token == endDelim) {
      return;
    } else {
      switch (_token) {
        case TOKEN_END:
        case TOKEN_ERROR:
          return;
        case TOKEN_LPAREN:
          next();
          skipToCloseDelim(TOKEN_RPAREN, TOKEN_END);
          break;
        case TOKEN_LBRACKET:
          next();
          skipToCloseDelim(TOKEN_RBRACKET, TOKEN_END);
          break;
        case TOKEN_LBRACE:
          next();
          skipToCloseDelim(TOKEN_RBRACE, TOKEN_END);
          break;
        default:
          next();
          break;
      }
    }
  }
}

void Parser::skipToCloseParen() {
  for (;;) {
    if (_token == TOKEN_RPAREN) {
      next();
      return;
    } else if (_token == TOKEN_LPAREN) {
      next();
      skipToCloseParen();
      return;
    } else if (_token == TOKEN_END || _token == TOKEN_LBRACE || _token == TOKEN_LBRACKET
        || _token == TOKEN_ERROR) {
      return;
    }

    next();
  }
}

void Parser::unexpectedToken() {
  diag::error(_lexer.tokenLocation()) << "Unexpected token " << getTokenName(_token);
}

void Parser::expectedIdentifier() {
  expected("identifier");
}

void Parser::expectedCloseParen() {
  expected("closing ')'");
}

void Parser::expectedCloseBracket() {
  expected("closing ']'");
}

void Parser::expected(const char * what) {
  if (_token == TOKEN_ERROR) {
    lexerError();
  } else {
    diag::error(_lexer.tokenLocation()) << "Expected " << what << ", not " << getTokenName(_token);
  }
}

void Parser::lexerError() {
  Location loc = _lexer.tokenLocation();
  switch (_lexer.errorCode()) {
    case Lexer::ILLEGAL_CHAR:
      diag::error(loc) << "Illegal character: " << _lexer.tokenValueStr();
      break;

    case Lexer::UNTERMINATED_STRING:
      diag::error(loc) << "Unterminated string";
      break;

    case Lexer::MALFORMED_ESCAPE_SEQUENCE:
      diag::error(loc) << "Malformed string escape sequence";
      break;

    case Lexer::INVALID_UNICODE_CHAR:
      diag::error(loc) << "Invalid unicode character";
      break;

    default:
      break;
  }
}

}
