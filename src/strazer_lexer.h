#ifndef _STRAZER_LEXER_H
#define _STRAZER_LEXER_H

#include <string>

#include "strazer_token_kinds.h"

namespace strazer {

class Token;

class Lexer {
 public:
  Lexer(const std::string& file);
  Lexer(const char* buf_start, const char* buf_ptr, const char* buf_end);

 private:
  static bool IsObviouslySimpleCharacter(char c) { return '\\' != c; }

  bool Lex(Token& result);
  void SkipWhitespace();
  void FormTokenWithChars(Token& result, const char* tok_end,
                          tok::TokenKind kind);

  inline char GetAndAdvanceChar(const char* ptr) {
    if (IsObviouslySimpleCharacter(ptr[0])) return *ptr++;

    size_t size = 0;
    char c = GetCharAndSizeSlow(ptr, size);
    ptr += size;
    return c;
  }

  const char* ConsumeChar(const char* ptr, size_t size) {
    if (1 == size) return ptr + size;

    size = 0;
    GetCharAndSizeSlow(ptr, size);
    return ptr + size;
  }

  inline char GetCharAndSize(const char* ptr, size_t& size) {
    if (IsObviouslySimpleCharacter(ptr[0])) {
      size = 1;
      return *ptr;
    }

    size = 0;
    return GetCharAndSizeSlow(ptr, size);
  }

  inline char GetCharAndSizeSlow(const char* ptr, size_t& size) {
    if ('\\' == ptr[0]) ++size;
    return '\\';
  }

  bool LexNumericConstant(Token& result, const char* cur_ptr);
  bool LexStringLiteral(Token& result, const char* cur_ptr,
                        tok::TokenKind kind);
  bool LexCharConstant(Token& result, const char* cur_ptr, tok::TokenKind kind);

  const char* buf_start_;
  const char* buf_ptr_;
  const char* buf_end_;
};

}  // namespace strazer

#endif
