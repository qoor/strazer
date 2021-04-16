#include "strazer_lexer.h"

#include "strazer_cursor.h"

namespace strazer {
namespace lexer {
class CursorImpl : public Cursor {
 public:
  tok::TokenKind AdvanceToken() {
    char first_char = Bump();
    tok::TokenKind kind;
    size_t size;

    if ('/' == first_char) {
      if ('*' == First()) BlockComment();
    } else if (IsWhitespace(first_char)) {
      Whitespace();
    } else if (IsIdStart(first_char)) {
      Ident();
    } else if (isdigit(first_char)) {
      kind = Number();
      size = LenConsumed();
      EatLiteralSuffix();
    } else {
      switch (first_char) {
        case ';': {
          kind = tok::kSemi;
          break;
        }
        case ',': {
          kind = tok::kComma;
          break;
        }
        case '.': {
          kind = tok::kDot;
          break;
        }
        case '(': {
          kind = tok::kOpenParen;
          break;
        }
        case ')': {
          kind = tok::kCloseParen;
          break;
        }
        case '{': {
          kind = tok::kOpenBrace;
          break;
        }
        case '}': {
          kind = tok::kCloseBrace;
          break;
        }
        case '[': {
          kind = tok::kOpenBracket;
          break;
        }
        case ']': {
          kind = tok::kCloseBracket;
          break;
        }
        case '@': {
          kind = tok::kAt;
          break;
        }
        case '~': {
          kind = tok::kTilde;
          break;
        }
        case '?': {
          kind = tok::kQuestion;
          break;
        }
        case ':': {
          kind = tok::kColon;
          break;
        }
        case '=': {
          kind = tok::kEq;
          break;
        }
        case '!': {
          kind = tok::kBang;
          break;
        }
        case '<': {
          kind = tok::kLt;
          break;
        }
        case '>': {
          kind = tok::kGt;
          break;
        }
        case '-': {
          kind = tok::kMinus;
          break;
        }
        case '|': {
          kind = tok::kOr;
          break;
        }

        case '\'': {
          kind = Char();
          break;
        }
        case '"': {
          terminated = DoubleQuotedString();
          size = LenConsumed();
          if (terminated) EatLiteralSuffix();

          kind = tok::kLiteralString;
          break;
        }
      }
    }

    return Token(kind, LenConsumed());
  }
};
}  // namespace lexer
}  // namespace strazer
