/* ================================================================ *
   The Parser class.
 * ================================================================ */

#ifndef MINT_PARSE_PARSER_H
#define MINT_PARSE_PARSER_H

#ifndef MINT_LEX_LEXER_H
#include "mint/lex/Lexer.h"
#endif

#ifndef MINT_SUPPORT_POOL_H
#include "mint/support/Pool.h"
#endif

namespace mint {

class Node;
class Module;
class Object;
class Oper;
class String;

/** -------------------------------------------------------------------------
    Parser for Mint configuration files.
 */
class Parser {
public:
  typedef SmallVector<Node *, 16> NodeList;

  Parser(TextBuffer * src);

  /// True if we are finished parsing.
  bool finished() const { return _token == TOKEN_END; }

  /// Parse the module, and return a parse tree of all of the definitions.
  Oper * parseModule(Module * module);
  Node * option();

  /// Parsing rules specific to build configuration files.
  Oper * parseConfig();
  Node * projectConfig();

  Node * expression();
  Node * binaryOperator();
  Node * unaryOperator();
  Node * objectExpression();
  Node * primaryExpression();

  bool parseArgumentList(NodeList & args);

  Node * parseObjectLiteral(Node * prototype);
  Node * parseObjectParam(bool lazy);
  Node * parseDictionaryLiteral();
  Node * parseListLiteral();
  Node * parseIntegerLiteral();
  Node * parseFloatLiteral();
  Node * parseStringLiteral(bool interpolated);

private:
  /// Read the next token.
  void next();

  /// Match a token.
  bool match(Token tok);

  /// Match an identifier.
  String * matchIdent();

  /// Skip until we find an open brace or bracket.
  void skipToNextOpenDelim();

  /// Skip until we find an open brace or bracket, or the specified token.
  void skipToRParen();

  /// Skip until end of line (but also over braces and brackets).
  void skipToEndOfLine();

  // Error message functions.

  void expectedIdentifier();
  void expectedCloseParen();
  void expectedCloseBracket();
  void unexpectedToken();
  void lexerError();

  /// Error message that indicates we expected something else here.
  void expected(const char * what);

  Module * _module;         // The module we're parsing.
  Lexer  _lexer;            // Lexer
  Token _token;             // Current token
  Location _tokenLoc;       // Location of just-matched token
  bool _recover;            // In error recovery state.
};

}

#endif // MINT_PARSE_PARSER_H
