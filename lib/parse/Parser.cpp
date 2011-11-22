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
#include "mint/graph/TypeRegistry.h"

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

Oper * Parser::parseModule(Module * module) {
  NodeList args;
  while (diag::errorCount() == 0) {
    switch (_token) {
      case TOKEN_END:
        goto done;

      case TOKEN_IMPORT:
        diag::error(_lexer.tokenLocation()) << "Unimplemented: parseModule() / import";
        next();
        break;

      case TOKEN_OPTION: {
        next();
        if (Node * opt = option()) {
          args.push_back(opt);
        }
        break;
      }

      case TOKEN_IDENT: {
        Node * propName = primaryExpression();
        if (propName == NULL) {
          skipToEndOfLine();
          continue;
        }
        if (!match(TOKEN_ASSIGN)) {
          expected("assignment");
          skipToEndOfLine();
          continue;
        }
        Node * propValue = expression();
        if (propValue == NULL) {
          skipToEndOfLine();
          continue;
        }

        Node * propArgs[] = { propName, propValue };
        args.push_back(
            Oper::create(Node::NK_SET_MEMBER,
                propName->location() | propValue->location(), NULL, propArgs));
        break;
      }

      default:
        expected("definition");
        return NULL;
    }
  }
done:
  return Oper::create(Node::NK_MAKE_MODULE, module->location(), NULL, args);
}

Node * Parser::option() {
  Location loc = _tokenLoc;
  String * optName = matchIdent();
  if (optName == NULL) {
    expected("option name");
    skipToEndOfLine();
    return NULL;
  }
  Node * optType = NULL;
  if (match(TOKEN_COLON)) {
    optType = primaryExpression();
    if (optType == NULL) {
      expected("option type");
      skipToEndOfLine();
      return NULL;
    }
  }
  NodeList args;
  args.push_back(optName);
  args.push_back(optType);
  if (match(TOKEN_LBRACE)) {
    while (!match(TOKEN_RBRACE)) {
      String * propName = matchIdent();
      if (propName == NULL) {
        expected("option parameter");
        skipToEndOfLine();
        continue;
      }
      if (!match(TOKEN_ASSIGN)) {
        expected("assignment");
      }
      Node * propValue = expression();
      if (propValue == NULL) {
        skipToEndOfLine();
        continue;
      }
      Node * propArgs[] = { propName, propValue };
      args.push_back(
          Oper::create(Node::NK_SET_MEMBER,
              propName->location() | propValue->location(), NULL, propArgs));
    }
  }
  loc |= _tokenLoc;
  return Oper::create(Node::NK_MAKE_OPTION, loc, NULL, args);
}

Oper * Parser::parseConfig() {
  NodeList args;
  while (diag::errorCount() == 0) {
    switch (_token) {
      case TOKEN_END:
        goto done;

      case TOKEN_PROJECT: {
        next();
        if (Node * opt = projectConfig()) {
          args.push_back(opt);
        }
        break;
      }

      default:
        expected("project configuration");
        return NULL;
    }
  }
done:
  return Oper::create(Node::NK_MAKE_MODULE, Location(), NULL, args);
}

Node * Parser::projectConfig() {
  NodeList args;
  if (!match(TOKEN_LBRACE)) {
    expected("project configuration");
    return NULL;
  }
  while (!match(TOKEN_RBRACE)) {
    if (match(TOKEN_OPTION)) {
      if (Node * opt = option()) {
        args.push_back(opt);
      }
    } else {
      String * propName = matchIdent();
      if (propName == NULL) {
        expected("project parameter");
        skipToEndOfLine();
        continue;
      }
      if (!match(TOKEN_ASSIGN)) {
        expected("assignment");
      }
      Node * propValue = expression();
      if (propValue == NULL) {
        skipToEndOfLine();
        continue;
      }
      Node * propArgs[] = { propName, propValue };
      args.push_back(
          Oper::create(Node::NK_SET_MEMBER,
              propName->location() | propValue->location(), NULL, propArgs));
    }
  }
  return Oper::create(Node::NK_PROJECT, Location(), NULL, args);
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

//      case Token_Ampersand:
//        opstack.pushOperator(callOperator(
//              &ASTIdent::operatorBitAnd, loc), Prec_BitAnd);
//        next();
//        break;
//
//      case Token_Bar:
//        opstack.pushOperator(callOperator(
//              &ASTIdent::operatorBitOr, loc), Prec_BitOr);
//        next();
//        break;
//
//      case Token_Caret:
//        opstack.pushOperator(callOperator(
//              &ASTIdent::operatorBitXor, loc), Prec_BitXor);
//        next();
//        break;
//
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

//      case Token_LogicalNot: {
//        // Negated operators
//        next();
//        Location loc = loc;
//        if (match(Token_In)) {
//          opstack.pushOperator(
//            new ASTOper(ASTNode::NotIn, loc), Prec_Contains);
//        } else {
//          diag.error(loc) << "'in' expected after 'not'";
//        }
//        break;
//      }

      //case Token_DoubleAmp:
      //case Token_DoubleBar:
      //    break;
      default:
        goto done;
        /*if (!opstack.reduceAll()) {
          return e0;
        }

        return opstack.getExpression();*/
    }

    Node * e1 = unaryOperator();
    if (e1 == NULL) {
      // Special case for pointer declaration.

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
//    case Token_LogicalNot: {
//      // Negated operators
//      next();
//      Location loc = lexer.tokenLocation();
//      ASTNode * e1 = unaryOperator();
//      if (e1 == NULL)
//        return NULL;
//      ASTOper * result = new ASTOper(ASTNode::LogicalNot, loc);
//      result->append(e1);
//      return result;
//    }
//
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
      return objectExpression();
  }
}

Node * Parser::objectExpression() {
  Node * result = primaryExpression();
  if (result && match(TOKEN_LBRACE)) {
    result = parseObjectLiteral(result);
  }

  return result;
}

Node * Parser::primaryExpression() {
  Node * result = NULL;

  Location loc = _lexer.tokenLocation();
  switch (int(_token)) {
    case TOKEN_LPAREN:
      next();
      result = expression();
      //result = expressionList();
      // Match generator expression here...
      if (!match(TOKEN_RPAREN)) {
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

    case TOKEN_SUPER: {
      next();
      result = new Node(Node::NK_SUPER, loc, NULL);
      break;
    }

    case TOKEN_DQ_STRING:
      result = parseStringLiteral(true);
      break;

    case TOKEN_SQ_STRING:
      result = parseStringLiteral(false);
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

//    case Token_Function: {
//      next();
//      ASTFunctionDecl * fn = functionDeclaration(ASTNode::AnonFn, "$call", DeclModifiers());
//      if (_token == Token_LBrace) {
//        ASTFunctionDecl * saveFunction = function;
//        function = fn;
//        Stmt * body = bodyStmt();
//        function = saveFunction;
//        fn->setBody(body);
//      }
//
//      result = fn;
//      break;
//    }

    case TOKEN_LBRACKET:
      next();
      result = parseListLiteral();
      break;

//    case Token_If:
//      next();
//      return ifStmt();
//
//    case Token_Switch:
//      next();
//      return switchStmt();
//
//    case Token_Match:
//      next();
//      return matchStmt();

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
      result = new Node(Node::NK_TYPENAME, loc, NULL);
      break;
    }
//      DEFINE_TOKEN(TYPENAME_LIST)
//      DEFINE_TOKEN(TYPENAME_DICT)
  }

  // Suffix operators
  if (result) {
    for (;;) {
      loc = _lexer.tokenLocation();
      if (match(TOKEN_LPAREN)) {
        // Call
        NodeList args;
        args.push_back(result);
        loc = result->location();
        if (!parseArgumentList(args)) {
          return NULL;
        }
        for (NodeList::const_iterator it = args.begin(); it != args.end(); ++it) {
          loc |= (*it)->location();
        }
        result = Oper::create(Node::NK_CALL, loc, NULL, args);
      } else if (match(TOKEN_LBRACKET)) {
        // Array dereference
        Node * arg = expression();
        if (arg == NULL) {
          return NULL;
        }
        if (!match(TOKEN_RBRACKET)) {
          expectedCloseBracket();
          return NULL;
        }
        Node * args[2] = { result, arg };
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

bool Parser::parseArgumentList(NodeList & args) {
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

    if (match(TOKEN_RPAREN)) {
      return true;
    } else if (match(TOKEN_COMMA)) {
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
    if (_token == TOKEN_LAZY) {
      next();
      if (!match(TOKEN_PARAM)) {
        expected("'param'");
      }
      Node * param = parseObjectParam(true);
      if (param == NULL) {
        skipToEndOfLine();
        continue;
      }
      args.push_back(param);
    } else if (_token == TOKEN_PARAM) {
      next();
      Node * param = parseObjectParam(false);
      if (param == NULL) {
        skipToEndOfLine();
        continue;
      }
      args.push_back(param);
    } else if (_token == TOKEN_IDENT) {
      Node * propName = primaryExpression();
      if (propName == NULL) {
        skipToEndOfLine();
        continue;
      }
      if (!match(TOKEN_ASSIGN)) {
        expected("assignment");
      }
      Node * propValue = expression();
      if (propValue == NULL) {
        skipToEndOfLine();
        continue;
      }

      Node * propArgs[] = { propName, propValue };
      args.push_back(
          Oper::create(Node::NK_SET_MEMBER,
              propName->location() | propValue->location(), NULL, propArgs));
    } else {
      diag::error(_lexer.tokenLocation()) << "Expected object property";
      skipToEndOfLine();
    }
  }
  loc |= _tokenLoc;
  return Oper::create(Node::NK_MAKE_OBJECT, loc, NULL, args);
}

Node * Parser::parseObjectParam(bool lazy) {
  Location loc = _tokenLoc;
  Node * name = matchIdent();
  Node * type = NULL;
  if (name == NULL) {
    expected("parameter name");
    return NULL;
  }
  if (match(TOKEN_COLON)) {
    type = expression();
  }
  if (!match(TOKEN_ASSIGN)) {
    expected("assignment");
    return NULL;
  }
  Node * propValue = expression();
  if (propValue == NULL) {
    skipToEndOfLine();
    return NULL;
  }
  loc |= _tokenLoc;
  Node * propFlags = new Literal<int>(Node::NK_INTEGER, Location(), NULL, lazy ? 1 : 0);
  Node * propArgs[] = { name, type, propValue, propFlags };
  return Oper::create(Node::NK_MAKE_PARAM, loc, NULL, propArgs);
}

Node * Parser::parseDictionaryLiteral() {
  diag::error(_lexer.tokenLocation()) << "Unimplemented: parseDictionaryLiteral()";
  return NULL;
}

Node * Parser::parseListLiteral() {
  NodeList args;
  Location loc = _tokenLoc;
  if (!match(TOKEN_RBRACKET)) {
    for (;;) {
      Node * n = expression();
      if (n == NULL) {
        //expectedCloseBracket();
        //skipToRParen();
        return NULL;
      }
      args.push_back(n);

      if (match(TOKEN_RBRACKET)) {
        break;
      } else if (!match(TOKEN_COMMA)) {
        expectedCloseBracket();
        //skipToRParen();
      }
    }
  }
  loc |= _tokenLoc;
  return Oper::create(Node::NK_LIST, loc, NULL, args);
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

Node * Parser::parseStringLiteral(bool interpolated) {
  Location loc = _lexer.tokenLocation();
  String * value = String::create(
      Node::NK_STRING, loc, TypeRegistry::stringType(), _lexer.tokenValue());

  if (interpolated) {
  }
  next();
  return value;
}

void Parser::skipToNextOpenDelim() {
  for (;;) {
    if (_token == TOKEN_END || _token == TOKEN_LPAREN || _token == TOKEN_LBRACE ||
        _token == TOKEN_LBRACKET) {
      return;
    }

    next();
  }
}

void Parser::skipToEndOfLine() {
  while (!_lexer.lineBreakBefore()) {
    if (_token == TOKEN_END) {
      return;
    } else if (match(TOKEN_LPAREN)) {
      skipToRParen();
    } else if (match(TOKEN_LBRACE)) {
    } else if (match(TOKEN_LBRACKET)) {
    }

    next();
  }
}

void Parser::skipToRParen() {
  for (;;) {
    if (_token == TOKEN_RPAREN) {
      next();
      return;
    } else if (_token == TOKEN_LPAREN) {
      next();
      skipToRParen();
      return;
    } else if (_token == TOKEN_END || _token == TOKEN_LBRACE ||
        _token == TOKEN_LBRACKET) {
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
  diag::error(_lexer.tokenLocation()) << "Expected " << what << ", not " << getTokenName(_token);
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
