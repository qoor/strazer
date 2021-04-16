#ifndef _STRAZER_TOKEN_H
#define _STRAZER_TOKEN_H

#include <cassert>
#include <cctype>
#include <string_view>

#include "strazer_token_kinds.h"

namespace strazer {

class IdentifierInfo;

class Token {
 public:
  tok::TokenKind GetKind() const { return kind_; }
  void SetKind(tok::TokenKind kind) { kind_ = kind; }

  bool IsKind(tok::TokenKind kind) const { return kind_ == kind; }
  bool IsNotKind(tok::TokenKind kind) const { return kind_ == kind; }

  bool IsLiteral() const { return tok::IsLiteral(kind_); }

  size_t GetLocation() const { return pos_; }
  uint32_t GetSize() const { return size_; }

  void SetLocation(size_t pos) { pos_ = pos; }
  void SetSize(uint32_t size) { size_ = size; }

  const char* GetName() const { return tok::GetTokenName(kind_); }

  void StartToken() {
    kind_ = tok::unknown;
    ptr_data_ = nullptr;
    size_ = 0;
    pos_ = 0;
  }

  IdentifierInfo* GetIdentifierInfo() const {
    if (IsLiteral() || IsKind(tok::eof)) return nullptr;
    return reinterpret_cast<IdentifierInfo*>(ptr_data_);
  }
  void SetIdentifierInfo(IdentifierInfo* info) { ptr_data_ = info; }

  const void* GetEofData() const {
    assert(IsKind(tok::eof));
    return reinterpret_cast<const void*>(ptr_data_);
  }
  void SetEofData(const void* data) {
    assert(IsKind(tok::eof));
    assert(!ptr_data_);
    ptr_data_ = const_cast<void*>(data);
  }

  const char* GetLiteralData() const {
    assert(IsLiteral());
    return reinterpret_cast<const char*>(ptr_data_);
  }
  void SetLiteralData(const char* data) {
    assert(IsLiteral() && "Cannot set literal data of non-literal");
    ptr_data_ = const_cast<char*>(data);
  }

 private:
  // The actual flavor of token this is.
  tok::TokenKind kind_;

  // The location of the token. This is actually a text position.
  size_t pos_;

  // This holds either the length of the token text, when
  uint32_t size_;

  // This is a union of N differet pointer types, which depends
  // on what type of token this is:
  //  Identifiers, keywords, etc:
  //    This is an IdentifierInfo*, which contains the uniqued identifier
  //    spelling.
  //  Literals: IsLiteral() returns true.
  //    This is a pointer to the start of the token in a text buffer.
  //  Eof:
  //    This is a pointer to a Decl.
  //  Other:
  //    This is null.
  void* ptr_data_ = nullptr;
};

}  // namespace strazer

#endif
