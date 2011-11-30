/* ================================================================ *
   The Parser class.
 * ================================================================ */

#ifndef MINT_PARSE_PARSER_H
#define MINT_PARSE_PARSER_H

#ifndef MINT_LEX_LEXER_H
#include "mint/lex/Lexer.h"
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
  Node * importName();

  /// Parsing rules specific to build configuration files.
  Oper * parseConfig();
  Node * projectConfig();

  Node * expression();
  Node * binaryOperator();
  Node * unaryOperator();
  Node * objectExpression();
  Node * primaryExpression();

  Node * doStmt();
  Node * letStmt();

  bool parseArgumentList(NodeList & args, Location & l);

  Node * parseObjectLiteral(Node * prototype);
  Node * parseObjectParam(unsigned flags);
  Node * parseDictionaryLiteral();
  Node * parseListLiteral();
  Node * parseIntegerLiteral();
  Node * parseFloatLiteral();
  Node * parseStringLiteral();
  Node * parseInterpolatedStringLiteral();

private:
  /// Read the next token.
  void next();

  /// Match a token.
  bool match(Token tok);

  /// Match an identifier.
  String * matchIdent();

  /// Skip until we find an open brace or bracket.
  void skipToNextOpenDelim();

  /// Skip until we a close paren.
  void skipToCloseParen();

  /// Skip until we encounter either 'stopToken' (and consume it)
  /// or until we encounter 'endDelim' (and not consume it). In either
  /// case, also skip over matched pairs of parens, braces and brackets.
  void skipToCloseDelim(Token stopToken, Token endDelim);

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
