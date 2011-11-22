/* ================================================================== *
 * Diagnostics unit test
 * ================================================================== */

#include "gtest/gtest.h"
#include "mint/lex/Lexer.h"

namespace mint {

/// A function that scans a single token.
Token lexToken(const char * srcText) {
  TextBuffer src(srcText, strlen(srcText));
  Lexer lex(&src);

  Token result = lex.next();
  EXPECT_EQ(TOKEN_END, lex.next());
  return result;
}

/// A function that scans a single token and returns an error.
Token lexTokenError(const char * srcText) {
  TextBuffer src(srcText, strlen(srcText));
  Lexer lex(&src);
  return lex.next();
}

TEST(LexerTest, SingleTokens) {
  // Whitespace
  EXPECT_EQ(TOKEN_END, lexToken(""));
  EXPECT_EQ(TOKEN_END, lexToken(" "));
  EXPECT_EQ(TOKEN_END, lexToken("\t"));
  EXPECT_EQ(TOKEN_END, lexToken("\r\n"));

  // Idents
  EXPECT_EQ(TOKEN_IDENT, lexToken("_"));
  EXPECT_EQ(TOKEN_IDENT, lexToken("a"));
  EXPECT_EQ(TOKEN_IDENT, lexToken("z"));
  EXPECT_EQ(TOKEN_IDENT, lexToken("azAZ_01"));
  EXPECT_EQ(TOKEN_IDENT, lexToken(" z "));
  EXPECT_EQ(TOKEN_IDENT, lexToken("a:b"));
  EXPECT_EQ(TOKEN_IDENT, lexToken(":a"));
  EXPECT_EQ(TOKEN_IDENT, lexToken("a:"));

  // Numbers
  EXPECT_EQ(TOKEN_INTEGER, lexToken("0"));
  EXPECT_EQ(TOKEN_INTEGER, lexToken(" 0 "));
  EXPECT_EQ(TOKEN_INTEGER, lexToken("1"));
  EXPECT_EQ(TOKEN_INTEGER, lexToken("9"));
  EXPECT_EQ(TOKEN_INTEGER, lexToken("10"));
  EXPECT_EQ(TOKEN_INTEGER, lexToken("0x10af"));
  EXPECT_EQ(TOKEN_INTEGER, lexToken("0X10af"));
  EXPECT_EQ(TOKEN_FLOAT, lexToken("0."));
  EXPECT_EQ(TOKEN_FLOAT, lexToken(" 0. "));
  EXPECT_EQ(TOKEN_FLOAT, lexToken(".0"));
  EXPECT_EQ(TOKEN_FLOAT, lexToken("0e12"));
  EXPECT_EQ(TOKEN_FLOAT, lexToken("0e+12"));
  EXPECT_EQ(TOKEN_FLOAT, lexToken("0e-12"));
  EXPECT_EQ(TOKEN_FLOAT, lexToken("0.0e12"));

  // Grouping tokens
  EXPECT_EQ(TOKEN_LBRACE, lexToken("{"));
  EXPECT_EQ(TOKEN_RBRACE, lexToken("}"));
  EXPECT_EQ(TOKEN_LPAREN, lexToken("("));
  EXPECT_EQ(TOKEN_RPAREN, lexToken(")"));
  EXPECT_EQ(TOKEN_LBRACKET, lexToken("["));
  EXPECT_EQ(TOKEN_RBRACKET, lexToken("]"));

  // Delimiters
  EXPECT_EQ(TOKEN_SEMI, lexToken(";"));
//  EXPECT_EQ(TOKEN_COLON, lexToken(":"));
  EXPECT_EQ(TOKEN_COMMA, lexToken(","));
//  EXPECT_EQ(TOKEN_ATSIGN, lexToken("@"));

  // Operator tokens
  EXPECT_EQ(TOKEN_ASSIGN, lexToken("="));
  //EXPECT_EQ(Token_AssignOp, lexToken(""));
//  EXPECT_EQ(TOKEN_RETURNTYPE, lexToken("->"));
  EXPECT_EQ(TOKEN_PLUS, lexToken("+"));
  EXPECT_EQ(TOKEN_MINUS, lexToken("-"));
  EXPECT_EQ(TOKEN_STAR, lexToken("*"));
  EXPECT_EQ(TOKEN_SLASH, lexToken("/"));
//  EXPECT_EQ(TOKEN_AMPERSAND, lexToken("&"));
  EXPECT_EQ(TOKEN_PERCENT, lexToken("%"));
//  EXPECT_EQ(TOKEN_BAR, lexToken("|"));
//  EXPECT_EQ(TOKEN_CARET, lexToken("^"));
//  EXPECT_EQ(TOKEN_TILDE, lexToken("~"));
  EXPECT_EQ(TOKEN_EXCLAM, lexToken("!"));
//  EXPECT_EQ(TOKEN_QMARK, lexToken("?"));
  EXPECT_EQ(TOKEN_DOUBLE_PLUS, lexToken("++"));
//  EXPECT_EQ(TOKEN_DECREMENT, lexToken("--"));

  // Relational operators
  EXPECT_EQ(TOKEN_LESS, lexToken("<"));
  EXPECT_EQ(TOKEN_GREATER, lexToken(">"));
  EXPECT_EQ(TOKEN_LESS_EQUAL, lexToken("<="));
  EXPECT_EQ(TOKEN_GREATER_EQUAL, lexToken(">="));
  EXPECT_EQ(TOKEN_EQUAL, lexToken("=="));
  EXPECT_EQ(TOKEN_NOTEQUAL, lexToken("!="));

//  EXPECT_EQ(Token_LShift, lexToken("<<"));
//  EXPECT_EQ(Token_RShift, lexToken(">>"));
  //EXPECT_EQ(Token_Scope, lexToken(""));

  // Joiners
  EXPECT_EQ(TOKEN_DOT, lexToken("."));
//  EXPECT_EQ(TOKEN_RANGE, lexToken(".."));
//  EXPECT_EQ(TOKEN_ELLIPSIS, lexToken("..."));

  // Operator keywords
//  EXPECT_EQ(Token_LogicalAnd, lexToken("and"));
//  EXPECT_EQ(Token_LogicalOr, lexToken("or"));
//  EXPECT_EQ(Token_LogicalNot, lexToken("not"));
//  EXPECT_EQ(Token_As, lexToken("as"));
//  EXPECT_EQ(Token_Is, lexToken("is"));
//  EXPECT_EQ(Token_In, lexToken("in"));

  // Primtypes
  EXPECT_EQ(TOKEN_TYPENAME_BOOL, lexToken("bool"));
  EXPECT_EQ(TOKEN_TYPENAME_INT, lexToken("int"));
  EXPECT_EQ(TOKEN_TYPENAME_FLOAT, lexToken("float"));

  EXPECT_EQ(TOKEN_IMPORT, lexToken("import"));

  // Statement keywords
//  EXPECT_EQ(Token_If, lexToken("if"));
//  EXPECT_EQ(Token_Else, lexToken("else"));
//  EXPECT_EQ(Token_Repeat, lexToken("repeat"));
//  EXPECT_EQ(Token_For, lexToken("for"));
//  EXPECT_EQ(Token_While, lexToken("while"));
//  EXPECT_EQ(Token_Return, lexToken("return"));

//  EXPECT_EQ(Token_Try, lexToken("try"));
//  EXPECT_EQ(Token_Catch, lexToken("catch"));
//  EXPECT_EQ(Token_Finally, lexToken("finally"));
//  EXPECT_EQ(Token_Switch, lexToken("switch"));
//  EXPECT_EQ(Token_Match, lexToken("match"));

  // String literals
  EXPECT_EQ(TOKEN_DQ_STRING, lexToken("\"\""));
  EXPECT_EQ(TOKEN_SQ_STRING, lexToken("'a'"));

  // Erroneous tokens
  EXPECT_EQ(TOKEN_ERROR, lexTokenError("@"));
}

TEST(LexerTest, StringLiterals) {

  {
    TextBuffer  src("\"\"");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_DQ_STRING, lex.next());
    EXPECT_EQ((size_t)0, lex.tokenValue().size());
  }

  {
    TextBuffer  src("\"abc\\n\\r\\$\"");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_DQ_STRING, lex.next());
    EXPECT_EQ("abc\n\r$", lex.tokenValueStr());
  }

  {
    StringRef     expected("\x01\xAA\xBB");
    TextBuffer  src("\"\\x01\\xAA\\xBB\"");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_DQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }

#if !_MSC_VER
  {
    StringRef     expected("\x01\u00AA\u00BB");
    TextBuffer  src("\"\\x01\\uAA\\uBB\"");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_DQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }

  {
    StringRef     expected("\u2105");
    TextBuffer  src("\"\\u2105\"");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_DQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }

  {
    StringRef     expected("\U00012100");
    TextBuffer  src("\"\\U00012100\"");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_DQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }
#endif
}

#if 0
TEST(LexerTest, CharLiterals) {

  {
    TextBuffer  src("\'a\'");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_SQ_STRING, lex.next());
    EXPECT_EQ((size_t)1, lex.tokenValue().size());
  }

  {
    StringRef     expected("\x01");
    TextBuffer  src("'\\x01'");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_SQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }

  {
    StringRef     expected("\xAA");
    TextBuffer  src("'\\xAA'");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_SQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }

  {
    StringRef     expected("000000aa");
    TextBuffer  src("'\\uAA'");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_SQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }

  {
    StringRef     expected("00002100");
    TextBuffer  src("'\\u2100\'");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_SQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }

  {
    StringRef     expected("00012100");
    TextBuffer  src("'\\U00012100'");
    Lexer           lex(&src);

    EXPECT_EQ(TOKEN_SQ_STRING, lex.next());
    EXPECT_EQ(expected, lex.tokenValueStr());
  }
}
#endif

TEST(LexerTest, Comments) {

  // Comments
  EXPECT_EQ(TOKEN_END, lexToken("#"));
  EXPECT_EQ(TOKEN_END, lexToken("# comment\n"));
  EXPECT_EQ(TOKEN_END, lexToken(" #\n "));
  EXPECT_EQ(TOKEN_INTEGER, lexToken("  # comment\n 10 # comment\n "));
  EXPECT_EQ(TOKEN_INTEGER, lexToken("  # comment\n10# comment\n "));
}

TEST(LexerTest, Location) {

  TextBuffer  src("\n\n   aaaaa    ");
  Lexer           lex(&src);

  EXPECT_EQ(TOKEN_IDENT, lex.next());

  EXPECT_EQ(5u, lex.tokenLocation().begin);
  EXPECT_EQ(10u, lex.tokenLocation().end);

  unsigned lineIndex = src.findContainingLine(lex.tokenLocation().begin);
  EXPECT_EQ(1u, lineIndex);

//  StringRef line;
//  EXPECT_TRUE(src.readLineAt(2, line));
//  EXPECT_EQ("   aaaaa    ", line);
}

}
