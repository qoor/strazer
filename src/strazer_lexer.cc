#include "strazer_lexer.h"

#include <memory>

#include "llvm/Support/ConvertUTF.h"
#include "strazer_token.h"
#include "strazer_utils.h"

using namespace strazer;

Lexer::Lexer(const char* buf_start, const char* buf_ptr, const char* buf_end) {
  InitLexer(buf_start, buf_ptr, buf_end);
}

void Lexer::InitLexer(const char* buf_start, const char* buf_ptr,
                      const char* buf_end) {
  buf_start_ = buf_start;
  buf_ptr_ = buf_ptr;
  buf_end_ = buf_end;

  assert(0 == buf_end[0] &&
         "We assume that the input buffer has a null character at the end to "
         "simplify lexing!");

  idents_.AddKeywords();
}

bool Lexer::Lex(Token& result) {
  result.StartToken();

LexNextToken:
  result.SetIdentifierInfo(nullptr);

  const char* cur_ptr = buf_ptr_;

  if (IsHorizontalWhitespace(*cur_ptr)) {
    do {
      ++cur_ptr;
    } while (IsHorizontalWhitespace(*cur_ptr));

    buf_ptr_ = cur_ptr;
  }

  size_t tmp, tmp2;

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
      SkipWhitespace(cur_ptr);
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
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
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
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case '_': {
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
    case '+': {
      c = GetCharAndSize(cur_ptr, tmp);
      if ('+' == c) {
        char after = GetCharAndSize(cur_ptr + tmp, tmp2);
        if ('+' == c) {
          kind = tok::plusplusplus;
          cur_ptr = ConsumeChar(ConsumeChar(cur_ptr, tmp), tmp2);
          break;
        }
      }
      [[fallthrough]];
    }
    case '-': {
      c = GetCharAndSize(cur_ptr, tmp);
      if ('>' == c)
        kind = tok::arrow;
      else
        kind = tok::minus;
      break;
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
        printf("comment start\n");
        if (SkipBlockComment(ConsumeChar(cur_ptr, tmp))) return true;
        printf("comment end\n");
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
      kind = tok::unknown;
      break;
    }
  }

  FormTokenWithChars(result, cur_ptr, kind);
  return true;
}

void Lexer::SkipWhitespace(const char* cur_ptr) {
  unsigned char c = *cur_ptr;

  while (true) {
    while (IsHorizontalWhitespace(c)) c = *++cur_ptr;

    if (!IsVerticalWhitespace(c)) break;

    c = *++cur_ptr;
  }

  buf_ptr_ = cur_ptr;
}

bool Lexer::LexIdentifier(Token& result, const char* cur_ptr) {
  size_t size;
  unsigned char c = *cur_ptr++;
  while (IsIdentifierBody(c)) c = *cur_ptr++;

  --cur_ptr;

  if (IsASCII(c) && '\\' != c) {
  finish_identifier:
    const char* id_start = buf_ptr_;
    FormTokenWithChars(result, cur_ptr, tok::raw_identifier);
    result.SetRawIdentifierData(id_start);

    LookUpIdentifierInfo(result);
    return true;
  }

  c = GetCharAndSize(cur_ptr, size);
  while (true) {
    if (!IsIdentifierBody(c)) goto finish_identifier;

    cur_ptr = ConsumeChar(cur_ptr, size);

    c = GetCharAndSize(cur_ptr, size);
    while (IsIdentifierBody(c)) {
      cur_ptr = ConsumeChar(cur_ptr, size);
      c = GetCharAndSize(cur_ptr, size);
    }
  }
}

bool Lexer::LexNumericConstant(Token& result, const char* cur_ptr) {
  size_t size;
  char c = GetCharAndSize(cur_ptr, size);
  while (IsPreprocessingNumberBody(c)) {
    cur_ptr = ConsumeChar(cur_ptr, size);
    c = GetCharAndSize(cur_ptr, size);
  }

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

  while (IsHorizontalWhitespace(*cur_ptr) || 0 == *cur_ptr) --cur_ptr;

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
    SkipWhitespace(cur_ptr);
    return false;
  }

  buf_ptr_ = cur_ptr;
  return false;
}

bool Lexer::IsHexaLiteral(const char* start) {
  size_t size;
  char c1 = GetCharAndSize(start, size);
  if ('0' != c1) return false;
  char c2 = GetCharAndSize(start + size, size);
  return ('x' == c2 || 'X' == c2);
}

IdentifierInfo* Lexer::LookUpIdentifierInfo(Token& identifier) const {
  assert(!identifier.GetRawIdentifier().empty() && "No raw identifier data!");

  IdentifierInfo* ident_info;
  ident_info = GetIdentifierInfo(identifier.GetRawIdentifier());
  identifier.SetIdentifierInfo(ident_info);
  identifier.SetKind(ident_info->GetTokenID());
  return ident_info;
}
