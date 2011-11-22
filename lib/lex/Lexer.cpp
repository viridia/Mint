/* ================================================================== *
 * Lexer
 * ================================================================== */

#include "mint/lex/Lexer.h"

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

namespace mint {

#ifdef DEFINE_TOKEN
#undef DEFINE_TOKEN
#endif

#define DEFINE_TOKEN(x) #x,
#define DEFINE_TOKEN_RANGE(x, start, end)

static const char * tokenNames[] = {
#include "mint/lex/Tokens.def"
};

const char * getTokenName(Token tk) {
  if (unsigned(tk) < (sizeof(tokenNames) / sizeof(tokenNames[0]))) {
    return tokenNames[unsigned(tk)];
  }
  return "<Invalid Token>";
}

#undef TYPE_KIND

namespace {
  // TODO: Add support for unicode letters.
  bool isNameStartChar(char ch) {
    return (ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z') ||
        (ch == '_' || ch == ':');
  }

  bool isNameChar(char ch) {
    return (ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') ||
        (ch == '_' || ch == ':');
  }

  bool isDigitChar(char ch) {
    return (ch >= '0' && ch <= '9');
  }

  bool isHexDigitChar(char ch) {
    return ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'));
  }

  // Instead of a hash table or sorted list, we rely on the compiler to
  // be smart about optimizing a switch statement that is known to
  // have a small number of cases.
  Token lookupKeyword(StringRef kw) {
    char ch = kw[0];
    if (ch >= 'a' && ch <= 'z') {
      switch ((unsigned char)ch) {
        case 'a':
          if (kw == "any") return TOKEN_TYPENAME_ANY;
          if (kw == "and") return TOKEN_AND;
          break;

        case 'b':
          if (kw == "bool") return TOKEN_TYPENAME_BOOL;
          break;

        case 'f':
          if (kw == "float") return TOKEN_TYPENAME_FLOAT;
          if (kw == "false") return TOKEN_FALSE;
          break;

        case 'i':
          if (kw == "int") return TOKEN_TYPENAME_INT;
          if (kw == "import") return TOKEN_IMPORT;
          break;

        case 'o':
          if (kw == "option") return TOKEN_OPTION;
          if (kw == "or") return TOKEN_OR;
          break;

        case 'p':
          if (kw == "param") return TOKEN_PARAM;
          if (kw == "project") return TOKEN_PROJECT;
          break;

        case 's':
          if (kw == "super") return TOKEN_SUPER;
          break;

        case 't':
          if (kw == "true") return TOKEN_TRUE;
          break;
      }
    }

    return TOKEN_IDENT;
  }
}

Lexer::Lexer(TextBuffer * buffer)
  : _buffer(buffer)
  , _pos(buffer->begin())
  , _end(buffer->end())
  , _errorCode(ERROR_NONE)
{
  _tokenLocation.source = buffer;
  _tokenLocation.begin = _tokenLocation.end = 0;
  _buffer->lineBreak(0, true);
  if (_pos < _end) {
    _ch = *_pos;
  } else {
    _ch = ~unsigned(0);
  }
}

inline void Lexer::readCh() {
  if (_pos < _end - 1) {
    _ch = *++_pos;
  } else {
    if (_pos < _end) {
      ++_pos;
    }
    _ch = ~unsigned(0);
  }
}

Token Lexer::next() {
  // Whitespace loop
  _lineBreakBefore = false;
  for (;;) {
    if (_pos >= _end) {  // EOF
      _buffer->lineBreak(_pos);
      return TOKEN_END;
    }

    if (_ch == ' ' || _ch == '\t' || _ch == '\b') { // Horizontal whitespace
      readCh();
    } else if (_ch == '\n') {  // Linefeed
      readCh();
      lineBreak();
      _lineBreakBefore = true;
    } else if (_ch == '\r') {  // Carriage return. Look for CRLF pair and count as 1 line.
      readCh();
      if (_ch == '\n') {
        readCh();
      }
      lineBreak();
      _lineBreakBefore = true;
    } else if (_ch == '#') { // Comment start
      readCh();
      while (_ch >= 0 && _ch != '\n' && _ch != '\r') {
        readCh();
      }
      _lineBreakBefore = true;
    } else {
      break;
    }
  }

  _tokenLocation.begin = unsigned(_pos - _buffer->begin());
  Token result = readToken();
  _tokenLocation.end = unsigned(_pos - _buffer->begin());
  return result;
}

Token Lexer::readToken() {

  // Identifier
  if (isNameStartChar(_ch)) {
    _tokenValue.clear();
    _tokenValue.push_back(_ch);
    readCh();
    while (isNameChar(_ch)) {
      _tokenValue.push_back(_ch);
      readCh();
    }

    if (_tokenValue.size() == 1 && _tokenValue[0] == ':') {
      return TOKEN_COLON;
    }

    // Check for keyword
    return lookupKeyword(StringRef(_tokenValue.data(), _tokenValue.size()));
  }

  // Number
  if (isDigitChar(_ch) || _ch == '.') {
    bool isFloat = false;
    _tokenValue.clear();

    // Hex number check
    if (_ch == '0') {
      _tokenValue.push_back('0');
      readCh();
      if (_ch == 'X' || _ch == 'x') {
        _tokenValue.push_back('x');
        readCh();
        for (;;) {
          if (isHexDigitChar(_ch)) {
            _tokenValue.push_back(_ch);
            readCh();
          } else if (_ch == '_') {
            readCh();
          } else {
            break;
          }
        }

        return TOKEN_INTEGER;
      }
    }

    // Integer part
    for (;;) {
      if (isDigitChar(_ch)) {
        _tokenValue.push_back(_ch);
        readCh();
      } else if (_ch == '_') {
        readCh();
      } else {
        break;
      }
    }

    // Fractional part
    if (_ch == '.') {
      readCh();

      // Check for case where this isn't a decimal point,
      // but just a dot token.
      if (!isDigitChar(_ch) && _tokenValue.empty()) {
        return TOKEN_DOT;
      }

      // It's a float
      isFloat = true;

      _tokenValue.push_back('.');
      for (;;) {
        if (isDigitChar(_ch)) {
          _tokenValue.push_back(_ch);
          readCh();
        } else if (_ch == '_') {
          readCh();
        } else {
          break;
        }
      }
    }

    // Exponent part
    if ((_ch == 'e' || _ch == 'E')) {
      isFloat = true;
      _tokenValue.push_back(_ch);
      readCh();
      if ((_ch == '+' || _ch == '-')) {
        _tokenValue.push_back(_ch);
        readCh();
      }
      for (;;) {
        if (isDigitChar(_ch)) {
          _tokenValue.push_back(_ch);
          readCh();
        } else if (_ch == '_') {
          readCh();
        } else {
          break;
        }
      }
    }

    if (isFloat) {
      return TOKEN_FLOAT;
    }

    return TOKEN_INTEGER;
  }

  // Punctuation
  switch (_ch) {
    case ':':
      readCh();
      return TOKEN_COLON;

    case '+':
      readCh();
      if (_ch == '+') {
        readCh();
        return TOKEN_DOUBLE_PLUS;
      }
      return TOKEN_PLUS;

    case '-':
      readCh();
//      if (_ch == '-') {
//        readCh();
//        return Token_Decrement;
//      }
//      if (_ch == '>') {
//        readCh();
//        return Token_ReturnType;
//      }
      return TOKEN_MINUS;

    case '*':
      readCh();
      return TOKEN_STAR;

    case '/':
      readCh();
      return TOKEN_SLASH;

    case '%':
      readCh();
      return TOKEN_PERCENT;

//    case '^':
//      readCh();
//      return Token_Caret;
//
//    case '|':
//      readCh();
//      return Token_Bar;
//
//    case '&':
//      readCh();
//      return Token_Ampersand;
//
//    case '~':
//      readCh();
//      return Token_Tilde;

    case '>':
      readCh();
      if (_ch == '=') {
        readCh();
        return TOKEN_GREATER_EQUAL;
      }
      return TOKEN_GREATER;

    case '<':
      readCh();
      if (_ch == '=') {
        readCh();
        return TOKEN_LESS_EQUAL;
      }
      return TOKEN_LESS;

    case '=':
      readCh();
      if (_ch == '=') {
        readCh();
        return TOKEN_EQUAL;
      }
      return TOKEN_ASSIGN;

    case '!':
      readCh();
      if (_ch == '=') {
        readCh();
        return TOKEN_NOTEQUAL;
      }
      return TOKEN_EXCLAM;

    case '{':
      readCh();
      return TOKEN_LBRACE;

    case '}':
      readCh();
      return TOKEN_RBRACE;

    case '[':
      readCh();
      return TOKEN_LBRACKET;

    case ']':
      readCh();
      return TOKEN_RBRACKET;

    case '(':
      readCh();
      return TOKEN_LPAREN;

    case ')':
      readCh();
      return TOKEN_RPAREN;

    case ';':
      readCh();
      return TOKEN_SEMI;

    case ',':
      readCh();
      return TOKEN_COMMA;

//    case '?':
//      readCh();
//      return Token_QMark;
//
//    case '$':
//      readCh();
//      return Token_DollarSign;
//
//    case '@':
//      readCh();
//      return Token_AtSign;

    case '"':
    case '\'': {
        // String literal
        _tokenValue.clear();
        char quote = _ch;
        int charCount = 0;
        readCh();
        for (;;) {
          if (_ch < 0) {
            _errorCode = UNTERMINATED_STRING;
            return TOKEN_ERROR;
          } else if (_ch == quote) {
            readCh();
            break;
          } else if (_ch == '\\') {
            readCh();
            switch (_ch) {
              case '0':
                _tokenValue.push_back('\0');
                readCh();
                break;
              case '\\':
                _tokenValue.push_back('\\');
                readCh();
                break;
              case '\'':
                _tokenValue.push_back('\'');
                readCh();
                break;
              case '\"':
                _tokenValue.push_back('\"');
                readCh();
                break;
              case 'r':
                _tokenValue.push_back('\r');
                readCh();
                break;
              case 'n':
                _tokenValue.push_back('\n');
                readCh();
                break;
              case 't':
                _tokenValue.push_back('\t');
                readCh();
                break;
              case 'b':
                _tokenValue.push_back('\b');
                readCh();
                break;
              case 'v':
                _tokenValue.push_back('\v');
                readCh();
                break;
              case 'x': {
                // Parse a hexidecimal character in a string.
                char charbuf[3];
                size_t  len = 0;
                readCh();
                while (isHexDigitChar(_ch) && len < 2) {
                  charbuf[len++] = _ch;
                  readCh();
                }

                if (len == 0) {
                  _errorCode = MALFORMED_ESCAPE_SEQUENCE;
                  return TOKEN_ERROR;
                }

                charbuf[len] = 0;
                long charVal = ::strtoul(charbuf, NULL, 16);
                _tokenValue.push_back(charVal);
                break;
              }

              case 'u':
              case 'U': {
                  // Parse a Unicode character literal in a string.
                  size_t maxLen = (_ch == 'u' ? 4 : 8);
                  char charbuf[9];
                  size_t len = 0;
                  readCh();
                  while (isHexDigitChar(_ch) && len < maxLen) {
                    charbuf[len++] = _ch;
                    readCh();
                  }
                  if (len == 0) {
                    // TODO: Report it
                    _errorCode = MALFORMED_ESCAPE_SEQUENCE;
                    return TOKEN_ERROR;
                  }

                  charbuf[len] = 0;
                  long charVal = ::strtoul(charbuf, NULL, 16);

                  if (!encodeUnicodeChar(charVal)) {
                    _errorCode = INVALID_UNICODE_CHAR;
                    return TOKEN_ERROR;
                  }

                  break;
                }
              default:
                _tokenValue.push_back(_ch);
                readCh();
                break;
            }
          } else if (_ch >= ' ') {
            _tokenValue.push_back(_ch);
            readCh();
          } else {
            _errorCode = MALFORMED_ESCAPE_SEQUENCE;
            return TOKEN_ERROR;
          }

          ++charCount;
        }

        if (quote == '\'') {
          return TOKEN_SQ_STRING;
        } else {
          return TOKEN_DQ_STRING;
        }

        return quote == '"' ? TOKEN_DQ_STRING : TOKEN_SQ_STRING;
      }

    default:
      break;
  }

  _tokenValue.push_back(_ch);
  _errorCode = ILLEGAL_CHAR;
  return TOKEN_ERROR;
}

bool Lexer::encodeUnicodeChar(long charVal) {
  if (charVal < 0x80) {
    _tokenValue.push_back(charVal);
  } else if (charVal < 0x800) {
    _tokenValue.push_back(0xc0 | (charVal >> 6));
    _tokenValue.push_back(0x80 | (charVal & 0x3f));
  } else if (charVal < 0x10000) {
    _tokenValue.push_back(0xe0 | (charVal >> 12));
    _tokenValue.push_back(0x80 | ((charVal >> 6) & 0x3f));
    _tokenValue.push_back(0x80 | (charVal & 0x3f));
  } else if (charVal < 0x100000) {
    _tokenValue.push_back(0xf0 | (charVal >> 18));
    _tokenValue.push_back(0x80 | ((charVal >> 12) & 0x3f));
    _tokenValue.push_back(0x80 | ((charVal >> 6) & 0x3f));
    _tokenValue.push_back(0x80 | (charVal & 0x3f));
  } else {
    _errorCode = INVALID_UNICODE_CHAR;
    return false;
  }

  return true;
}

}
