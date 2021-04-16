#include "strazer_lexer.h"

#include "strazer_token.h"
#include "strazer_utils.h"

using namespace strazer;

bool Lexer::Lex(Token& result) {
  result.StartToken();

LexNextToken:
  result.SetIdentifierInfo(nullptr);

  SkipWhitespace();
  const char* cur_ptr = buf_ptr_;

  size_t tmp;

  char c = GetAndAdvanceChar(cur_ptr);
  tok::TokenKind kind;

  switch (c) {
    case 0: {
      kind = tok::eof;
      break;
    }
    case '\r': {
      if ('\n' == cur_ptr[0]) GetAndAdvanceChar(cur_ptr);
      [[fallthrough]];
    }
    case '\n': {
      goto LexNextToken;
    }

    case ' ':
    case '\t':
    case '\f':
    case '\v': {
      goto LexNextToken;
    }

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      return LexNumericConstant(result, cur_ptr);
    }

    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K': /*'L'*/
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q': /*'R'*/
    case 'S':
    case 'T': /*'U'*/
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't': /*'u'*/
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case '_': {
      // Notify MIOpt that we read a non-whitespace/non-comment token.
      return LexIdentifier(result, cur_ptr);
    }

      // Character Constants.
    case '\'': {
      return LexCharConstant(result, cur_ptr, tok::char_constant);
    }

      // String Literals.
    case '"': {
      return LexStringLiteral(result, cur_ptr, tok::string_literal);
    }

      // Punctuators.
    case '?': {
      kind = tok::question;
      break;
    }
    case '[': {
      kind = tok::l_square;
      break;
    }
    case ']': {
      kind = tok::r_square;
      break;
    }
    case '(': {
      kind = tok::l_paren;
      break;
    }
    case ')': {
      kind = tok::r_paren;
      break;
    }
    case '{': {
      kind = tok::l_brace;
      break;
    }
    case '}': {
      kind = tok::r_brace;
      break;
    }
    case '*': {
      if ('*' == cur_ptr[0] && '*' == cur_ptr[1]) {
        kind = tok::triple_star;
        break;
      }
      [[fallthrough]];
    }
    case '~': {
      kind = tok::tilde;
      break;
    }
    case '!': {
      kind = tok::exclaim;
      break;
    }
    case '/': {
      c = GetCharAndSize(cur_ptr, tmp);
      if ('*' == c) {
        if (SkipBlockComment(result, ConsumeChar(cur_ptr, tmp))) return true;
        goto LexNextToken;
      } else {
        kind = tok::slash;
      }
      break;
    }
    case '<': {
      kind = tok::less;
      break;
    }
    case '>': {
      kind = tok::greater;
      break;
    }
    case '|': {
      kind = tok::pipe;
      break;
    }
    case ':': {
      kind = tok::colon;
      break;
    }
    case ',': {
      kind = tok::comma;
      break;
    }
    case '=': {
      kind = tok::equal;
      break;
    }

    default: {
      if (IsASCII(c)) {
        kind = tok::unknown;
        break;
      }
    }
  }

  FormTokenWithChars(result, cur_ptr, kind);
  return true;
}

void Lexer::SkipWhitespace() {
  unsigned char c = *buf_ptr_;
  while (IsHorizontalWhitespace(c)) c = *++buf_ptr_;
}

void Lexer::FormTokenWithChars(Token& result, const char* tok_end,
                               tok::TokenKind kind) {
  const size_t tok_len = tok_end - buf_ptr_;
  result.SetLocation(buf_ptr_ - buf_start_);
  result.SetKind(kind);
  result.SetSize(tok_len);
  buf_ptr_ = tok_end;
}

bool Lexer::LexNumericConstant(Token& result, const char* cur_ptr) {
  size_t size;
  char c = GetCharAndSize(cur_ptr, size);
  char prev_c = 0;
  while (IsPreprocessingNumberBody(c)) {
    cur_ptr = ConsumeChar(cur_ptr, size);
    prev_c = c;
    c = GetCharAndSize(cur_ptr, size);
  }

  if ('-' == c && !IsHexaLiteral(buf_ptr_))
    return LexNumericConstant(result, ConsumeChar(cur_ptr, size));

  const char* tok_start = buf_ptr_;
  FormTokenWithChars(result, cur_ptr, tok::numeric_constant);
  result.SetLiteralData(tok_start);
  return true;
}

bool Lexer::LexStringLiteral(Token& result, const char* cur_ptr,
                             tok::TokenKind kind) {
  char c = GetAndAdvanceChar(cur_ptr);
  while ('"' != c) {
    if ('\\' == c) c = GetAndAdvanceChar(cur_ptr);

    if ('\n' == c || '\r' == c || (0 == c && cur_ptr - 1 == buf_end_)) {
      FormTokenWithChars(result, cur_ptr - 1, tok::unknown);
      return true;
    }

    c = GetAndAdvanceChar(cur_ptr);
  }

  const char* tok_start = buf_ptr_;
  FormTokenWithChars(result, cur_ptr, kind);
  result.SetLiteralData(tok_start);
  return true;
}

bool Lexer::LexCharConstant(Token& result, const char* cur_ptr,
                            tok::TokenKind kind) {
  char c = GetAndAdvanceChar(cur_ptr);
  if ('\'' == c) {
    FormTokenWithChars(result, cur_ptr, tok::unknown);
    return true;
  }

  while ('\'' == c) {
    if ('\\' == c) c = GetAndAdvanceChar(cur_ptr);

    if ('\n' == c || '\r' == c || (0 == c && cur_ptr - 1 == buf_end_)) {
      FormTokenWithChars(result, cur_ptr - 1, tok::unknown);
      return true;
    }

    c = GetAndAdvanceChar(cur_ptr);
  }

  const char* tok_start = buf_ptr_;
  FormTokenWithChars(result, cur_ptr, kind);
  result.SetLiteralData(tok_start);
  return true;
}

static bool IsEndOfBlockCommentWithEscapedNewLine(const char* cur_ptr) {
  assert('\n' == cur_ptr[0] || '\r' == cur_ptr[0]);

  --cur_ptr;

  if ('\n' == cur_ptr[0] || '\r' == cur_ptr[0]) {
    if (cur_ptr[0] == cur_ptr[1]) return false;
    --cur_ptr;
  }

  bool has_space = false;
  while (IsHorizontalWhitespace(*cur_ptr) || 0 == *cur_ptr) {
    --cur_ptr;
    has_space = true;
  }

  if ('\\' == *cur_ptr) {
    if ('*' != cur_ptr[-1]) return false;
  } else {
    if ('/' != cur_ptr[0]) return false;

    cur_ptr -= 2;
  }

  return true;
}

bool Lexer::SkipBlockComment(const char* cur_ptr) {
  size_t char_size;
  unsigned char c = GetCharAndSize(cur_ptr, char_size);
  cur_ptr += char_size;
  if (0 == c && cur_ptr == buf_end_ - 1) {
    --buf_ptr_;
    return false;
  }

  if ('/' == c) c = *cur_ptr++;

  while (true) {
    if (cur_ptr + 24 < buf_end_) {
      while ('/' != c && 0 != ((intptr_t)cur_ptr & 0x0F)) c = *cur_ptr++;

      if ('/' == c) goto found_slash;

      while ('/' != cur_ptr[0] && '/' != cur_ptr[1] && '/' != cur_ptr[2] &&
             '/' != cur_ptr[3] && cur_ptr + 4 < buf_end_) {
        cur_ptr += 4;
      }

      c = *cur_ptr++;
    }

    while ('/' != c && '\0' != c) c = *cur_ptr++;

    if ('/' == c) {
    found_slash:
      if ('*' == cur_ptr[-2]) break;

      if ('\n' == cur_ptr[-2] || '\r' == cur_ptr[-2]) {
        if (IsEndOfBlockCommentWithEscapedNewLine(cur_ptr - 2)) break;
      }
    } else if (0 == c && cur_ptr == buf_end_ - 1) {
      --buf_ptr_;
      return false;
    }

    c = *cur_ptr++;
  }

  if (IsHorizontalWhitespace(*cur_ptr)) {
    SkipWhitespace();
    return false;
  }

  buf_ptr_ = cur_ptr;
  return false;
}
