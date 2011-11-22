/* ================================================================ *
   The Lexer class.
 * ================================================================ */

#ifndef MINT_LEX_LEXER_H
#define MINT_LEX_LEXER_H

#ifndef MINT_SUPPORT_TEXTBUFFER_H
#include "mint/support/TextBuffer.h"
#endif

#ifndef MINT_LEX_LOCATION_H
#include "mint/lex/Location.h"
#endif

#ifndef MINT_LEX_TOKENS_H
#include "mint/lex/Tokens.h"
#endif

namespace mint {

/** -------------------------------------------------------------------------
    Lexer for Mint configuration files.
 */
class Lexer {
public:
  enum LexerError {
    ERROR_NONE = 0,
    ILLEGAL_CHAR,
    UNTERMINATED_STRING,
    MALFORMED_ESCAPE_SEQUENCE,
    INVALID_UNICODE_CHAR,
  };

  /// Constructor
  Lexer(TextBuffer * buffer);

  /// Get the next token
  Token next();

  /// Current value of the token.
  const SmallVectorImpl<char> & tokenValue() const { return _tokenValue; }
  StringRef tokenValueStr() const {
    return StringRef(_tokenValue.data(), _tokenValue.size());
  }
  StringRef tokenValueCStr() {
    _tokenValue.push_back('\0');
    return StringRef(_tokenValue.data(), _tokenValue.size());
  }

  /// True if the whitespace immediately preceding the current token contains a line break.
  /// This will be true for the first token on every line except for the first line.
  bool lineBreakBefore() const { return _lineBreakBefore; }

  /// Location of the token in the source file.
  const Location & tokenLocation() { return _tokenLocation; }

  /// Current error code.
  LexerError errorCode() const { return _errorCode; }

private:
  // Read the next character.
  void readCh();
  void lineBreak() {
    _buffer->lineBreak(_pos);
  }
  Token readToken();
  bool encodeUnicodeChar(long charVal);

  TextBuffer * _buffer;         // Source file buffer.
  int _ch;                      // Most recently read character.
  TextBuffer::iterator _pos;    // Current read position.
  TextBuffer::iterator _end;    // End of input buffer.
  Location _tokenLocation;      // Location of current token.
  SmallVector<char, 64> _tokenValue; // Value of the current token.
  LexerError _errorCode;        // Error code.
  bool _lineBreakBefore;        // Line break flag
};

}

#endif // MINT_LEX_LEXER_H
