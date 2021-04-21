#ifndef _STRAZER_LEXER_H
#define _STRAZER_LEXER_H

#include <string>
#include <vector>

#include "strazer_identifier_table.h"
#include "strazer_token.h"
#include "strazer_token_kinds.h"
#include "strazer_utils.h"

namespace strazer {

class Lexer {
 public:
  Lexer(const char* buf_start, const char* buf_ptr, const char* buf_end);

  void InitLexer(const char* buf_start, const char* buf_ptr,
                 const char* buf_end);

  bool Lex(Token& result);

 private:
  static bool IsObviouslySimpleCharacter(char c) { return '\\' != c; }

  void SkipWhitespace(const char* cur_ptr);
  void FormTokenWithChars(Token& result, const char* tok_end,
                          tok::TokenKind kind) {
    const size_t tok_len = tok_end - buf_ptr_;
    assert(0 < tok_len && "Invalid token length");
    result.SetLocation(buf_ptr_ - buf_start_);
    result.SetKind(kind);
    result.SetLength(tok_len);
    buf_ptr_ = tok_end;
  }

  inline char GetAndAdvanceChar(const char*& ptr) {
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
    ++size;
    return *ptr;
  }

  bool LexIdentifier(Token& result, const char* cur_ptr);
  bool LexNumericConstant(Token& result, const char* cur_ptr);
  bool LexStringLiteral(Token& result, const char* cur_ptr,
                        tok::TokenKind kind);
  bool LexCharConstant(Token& result, const char* cur_ptr, tok::TokenKind kind);

  bool SkipBlockComment(const char* cur_ptr);

  bool IsHexaLiteral(const char* start);

  IdentifierInfo* LookUpIdentifierInfo(Token& identifier) const;

  IdentifierInfo* GetIdentifierInfo(std::string_view name) const {
    return &idents_.Get(name);
  }

  const char* buf_start_;
  const char* buf_ptr_;
  const char* buf_end_;

  mutable IdentifierTable idents_;
};

}  // namespace strazer

#endif
