/* ================================================================ *
   Definition of all lexical tokens.
 * ================================================================ */

#ifndef MINT_LEX_TOKENS_H
#define MINT_LEX_TOKENS_H

// Lexer

namespace mint {

#ifdef DEFINE_TOKEN
#undef DEFINE_TOKEN
#endif

#define DEFINE_TOKEN(x) TOKEN_##x,

enum Token {
  #include "Tokens.def"
  Token_Last
};

// Return the name of the specified token.
const char * getTokenName(Token tt);

}

#endif
