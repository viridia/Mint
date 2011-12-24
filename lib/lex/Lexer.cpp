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
          if (kw == "as") return TOKEN_AS;
          break;

        case 'b':
          if (kw == "bool") return TOKEN_TYPENAME_BOOL;
          break;

        case 'c':
          if (kw == "cached") return TOKEN_CACHED;
          break;

        case 'd':
          if (kw == "dict") return TOKEN_TYPENAME_DICT;
          if (kw == "do") return TOKEN_DO;
          if (kw == "def") return TOKEN_DEF;
          break;

        case 'e':
          if (kw == "else") return TOKEN_ELSE;
          break;

        case 'f':
          if (kw == "float") return TOKEN_TYPENAME_FLOAT;
          if (kw == "false") return TOKEN_FALSE;
          if (kw == "from") return TOKEN_FROM;
          break;

        case 'i':
          if (kw == "int") return TOKEN_TYPENAME_INT;
          if (kw == "import") return TOKEN_IMPORT;
          if (kw == "in") return TOKEN_IN;
          if (kw == "if") return TOKEN_IF;
          break;

        case 'l':
          if (kw == "list") return TOKEN_TYPENAME_LIST;
          if (kw == "let") return TOKEN_LET;
          break;

        case 'n':
          if (kw == "not") return TOKEN_NOT;
          break;

        case 'o':
          if (kw == "or") return TOKEN_OR;
          break;

        case 'p':
          if (kw == "param") return TOKEN_PARAM;
          if (kw == "project") return TOKEN_PROJECT;
          break;

        case 's':
          if (kw == "super") return TOKEN_SUPER;
          if (kw == "string") return TOKEN_TYPENAME_STRING;
          if (kw == "self") return TOKEN_SELF;
          break;

        case 't':
          if (kw == "true") return TOKEN_TRUE;
          break;

        case 'u':
          if (kw == "undefined") return TOKEN_UNDEFINED;
          break;

        case 'v':
          if (kw == "void") return TOKEN_TYPENAME_VOID;
          if (kw == "var") return TOKEN_VAR;
          break;
      }
    }

    return TOKEN_IDENT;
  }
}

void Location::trace() const {
  GC::safeMark(source);
}

Lexer::Lexer(TextBuffer * buffer)
  : _buffer(buffer)
  , _pos(buffer->begin())
  , _end(buffer->end())
  , _lexerState(START)
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
  switch (_lexerState) {
    case START:
    case INTERPOLATED_STRING_EXPR:
    case MULTILINE_STRING_EXPR:
      break;

    case INTERPOLATED_STRING: {
      _tokenLocation.begin = unsigned(_pos - _buffer->begin());
      Token result = readStringLiteral();
      _tokenLocation.end = unsigned(_pos - _buffer->begin());
      return result;
    }

    case MULTILINE_STRING: {
      _tokenLocation.begin = unsigned(_pos - _buffer->begin());
      Token result = readMultiLineStringLiteral();
      _tokenLocation.end = unsigned(_pos - _buffer->begin());
      return result;
    }

    case MULTILINE_STRING_END:
      _lexerState = START;
      return TOKEN_ISTRING_END;
  }

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

  // Only a limited set of punctuation is allowed in string interpolation expressions.
  if (_lexerState == INTERPOLATED_STRING_EXPR || _lexerState == MULTILINE_STRING_EXPR) {
    switch (_ch) {
      case '}':
        readCh();
        if (_lexerState == MULTILINE_STRING_EXPR) {
          _lexerState = MULTILINE_STRING;
          return readMultiLineStringLiteral();
        } else {
          _lexerState = INTERPOLATED_STRING;
          return readStringLiteral();
        }

      case '[':
        readCh();
        return TOKEN_LBRACKET;

      case ']':
        readCh();
        return TOKEN_RBRACKET;

      default:
        _tokenValue.push_back(_ch);
        _errorCode = ILLEGAL_CHAR;
        return TOKEN_ERROR;
    }
  }

  // Punctuation
  switch (_ch) {
    case ':':
      readCh();
      if (_ch == ':') {
        readCh();
        return TOKEN_DOUBLE_COLON;
      }
      return TOKEN_COLON;

    case '+':
      readCh();
      if (_ch == '+') {
        readCh();
        if (_ch == '=') {
          readCh();
          return TOKEN_DOUBLE_PLUS_ASSIGN;
        }
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

//    case '|':
//      readCh();
//      return TOKEN_PIPE;

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
      if (_ch == '{') {
        readCh();
        _lexerState = MULTILINE_STRING;
        return TOKEN_ISTRING_START;
      }
      return TOKEN_LESS;

    case '=':
      readCh();
      if (_ch == '=') {
        readCh();
        return TOKEN_EQUAL;
      }
      if (_ch == '>') {
        readCh();
        return TOKEN_MAPS_TO;
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

    // String literal with interpolation
    case '"':
      readCh();
      _lexerState = INTERPOLATED_STRING;
      return TOKEN_ISTRING_START;

    // String literal
    case '\'': {
      readCh();
      return readStringLiteral();
    }

    default:
      break;
  }

  _tokenValue.push_back(_ch);
  _errorCode = ILLEGAL_CHAR;
  return TOKEN_ERROR;
}

Token Lexer::readStringLiteral() {
  _tokenValue.clear();

  if (_ch == '"' && _lexerState == INTERPOLATED_STRING) {
    readCh();
    _lexerState = START;
    return TOKEN_ISTRING_END;
  }

  for (;;) {
    if (_ch < 0) {
      _errorCode = UNTERMINATED_STRING;
      _lexerState = START;
      return TOKEN_ERROR;
    } else if (_ch == '\'' && _lexerState == START) {
      readCh();
      return TOKEN_STRING;
    } else if (_ch == '\\') {
      readCh();
      Token tok = readEscapeChars();
      if (tok != TOKEN_STRING) {
        return tok;
      }
    } else if (_ch >= ' ') {
      if (_lexerState == INTERPOLATED_STRING) {
        // Reached the end of the istring.
        if (_ch == '"') {
          // Don't read the character, we'll read it next call.
          return TOKEN_STRING;
        } else if (_ch == '$') {
          readCh();
          if (_ch == '{') {
            readCh();
            _lexerState = INTERPOLATED_STRING_EXPR;
            if (_tokenValue.empty()) {
              return readToken();
            } else {
              return TOKEN_STRING;
            }
          }
          _tokenValue.push_back('$');
          continue;
        }
      }
      _tokenValue.push_back(_ch);
      readCh();
    } else {
      _errorCode = MALFORMED_ESCAPE_SEQUENCE;
      _lexerState = START;
      return TOKEN_ERROR;
    }
  }
}

Token Lexer::readMultiLineStringLiteral() {
  _tokenValue.clear();
  for (;;) {
    if (_ch < 0) {
      _errorCode = UNTERMINATED_STRING;
      _lexerState = START;
      return TOKEN_ERROR;
    } else if (_ch == '}') {
      readCh();
      if (_ch == '>') {
        readCh();
        if (_tokenValue.empty()) {
          _lexerState = START;
          return TOKEN_ISTRING_END;
        } else {
          _lexerState = MULTILINE_STRING_END;
          return TOKEN_STRING;
        }
      } else {
        _tokenValue.push_back('}');
      }
    } else if (_ch == '$') {
      readCh();
      if (_ch == '{') {
        readCh();
        _lexerState = MULTILINE_STRING_EXPR;
        if (_tokenValue.empty()) {
          return readToken();
        } else {
          return TOKEN_STRING;
        }
      }
      _tokenValue.push_back('$');
      continue;
    } else {
      _tokenValue.push_back(_ch);
      readCh();
    }
  }
}

Token Lexer::readEscapeChars() {
  // Assume that the initial backslash has already been read.
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

  return TOKEN_STRING;
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
