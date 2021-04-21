#ifndef _STRAZER_TOKEN_H
#define _STRAZER_TOKEN_H

#include <cassert>
#include <cctype>
#include <cstdio>
#include <string>
#include <string_view>

#include "strazer_identifier_table.h"
#include "strazer_token_kinds.h"

namespace strazer {

class IdentifierInfo;

class Token {
 public:
  tok::TokenKind GetKind() const { return kind_; }
  void SetKind(tok::TokenKind kind) { kind_ = kind; }

  bool IsKind(tok::TokenKind kind) const { return kind_ == kind; }
  bool IsNotKind(tok::TokenKind kind) const { return kind_ != kind; }
  bool IsOneOfKind(tok::TokenKind k1, tok::TokenKind k2) const {
    return kind_ == k1 || kind_ == k2;
  }

  bool IsAnyIdentifier() const { return tok::IsAnyIdentifier(GetKind()); }

  bool IsLiteral() const { return tok::IsLiteral(kind_); }

  size_t GetLocation() const { return pos_; }
  uint32_t GetLength() const { return size_; }

  void SetLocation(size_t pos) { pos_ = pos; }
  void SetLength(uint32_t size) { size_ = size; }

  const char* GetName() const { return tok::GetTokenName(kind_); }

  void StartToken() {
    kind_ = tok::unknown;
    ptr_data_ = nullptr;
    size_ = 0;
    pos_ = 0;
  }

  IdentifierInfo* GetIdentifierInfo() const {
    assert(IsNotKind(tok::raw_identifier) &&
           "GetIdentifierInfo() on a tok::raw_identifier token!");
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

  std::string_view GetRawIdentifier() const {
    assert(IsKind(tok::raw_identifier));
    return std::string_view(reinterpret_cast<const char*>(ptr_data_),
                            GetLength());
  }
  void SetRawIdentifierData(const char* ptr) {
    assert(IsKind(tok::raw_identifier));
    ptr_data_ = const_cast<char*>(ptr);
  }

  const char* GetLiteralData() const {
    assert(IsLiteral() && "Cannot get literal data of non-literal");
    return reinterpret_cast<const char*>(ptr_data_);
  }
  void SetLiteralData(const char* data) {
    assert(IsLiteral() && "Cannot set literal data of non-literal");
    ptr_data_ = const_cast<char*>(data);
  }

  void Print() const {
#ifndef NDEBUG
    if (IsKind(tok::identifier))
      printf("Token kind(%s) identifier(%s)\n", tok::GetTokenName(kind_),
             std::string(GetIdentifierInfo()->GetName()).c_str());
    else if (ptr_data_)
      printf("Token kind(%s) data(%s)\n", tok::GetTokenName(kind_),
             std::string((char*)ptr_data_, size_).c_str());
    else
      printf("Token kind(%s)\n", tok::GetTokenName(kind_));
#endif
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
