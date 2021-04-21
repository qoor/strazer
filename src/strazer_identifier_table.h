#ifndef _STRAZER_IDENTIFIER_TABLE_H
#define _STRAZER_IDENTIFIER_TABLE_H

#include <cassert>
#include <cstring>
#include <string_view>
#include <unordered_map>

#include "strazer_token_kinds.h"

namespace strazer {

class IdentifierInfo {
 public:
  IdentifierInfo(const IdentifierInfo&) = delete;
  IdentifierInfo& operator=(const IdentifierInfo&) = delete;
  IdentifierInfo(IdentifierInfo&&) = delete;
  IdentifierInfo& operator=(IdentifierInfo&&) = delete;

  template <size_t len>
  bool IsStr(const char (&str)[len]) const {
    return len - 1 == GetLength() && 0 == memcmp(GetNameStart(), str, len - 1);
  }

  bool IsStr(std::string_view str) const {
    std::string_view this_str(GetNameStart(), GetLength());
    return str == this_str;
  }

  const char* GetNameStart() const { return entry_->first.data(); }
  size_t GetLength() const { return entry_->first.size(); }
  std::string_view GetName() const {
    return std::string_view(GetNameStart(), GetLength());
  }

  tok::TokenKind GetTokenID() const {
    return static_cast<tok::TokenKind>(token_id_);
  }

  bool operator<(const IdentifierInfo& rhs) const {
    return GetName() < rhs.GetName();
  }

 private:
  friend class IdentifierTable;

  IdentifierInfo() = default;

  unsigned int token_id_ = tok::identifier;

  using IdentifierTableEntry =
      std::unordered_map<std::string_view, IdentifierInfo*>::const_iterator;
  IdentifierTableEntry entry_;
};

class IdentifierIterator {
 public:
  IdentifierIterator(const IdentifierIterator&) = delete;
  IdentifierIterator& operator=(const IdentifierIterator&) = delete;

  virtual ~IdentifierIterator() = default;

  virtual std::string_view Next() = 0;

 protected:
  IdentifierIterator() = default;
};

class IdentifierInfoLookup {
 public:
  virtual ~IdentifierInfoLookup() = default;

  virtual IdentifierInfo* Get(std::string_view name) = 0;

  virtual IdentifierIterator* GetIdentifiers();
};

class IdentifierTable {
 private:
  using HashTable = std::unordered_map<std::string_view, IdentifierInfo*>;

 public:
  IdentifierInfo& Get(std::string_view name) {
    auto entry = hash_table_.emplace(name, nullptr).first;

    IdentifierInfo*& ident_info = entry->second;
    if (ident_info) return *ident_info;

    ident_info = new IdentifierInfo();
    ident_info->entry_ = entry;
    return *ident_info;
  }

  using iterator = HashTable::const_iterator;
  using const_iterator = HashTable::const_iterator;

  IdentifierInfo& Get(std::string_view name, tok::TokenKind token_code) {
    IdentifierInfo& ident_info = Get(name);
    ident_info.token_id_ = token_code;
    assert(static_cast<unsigned int>(token_code) == ident_info.token_id_ &&
           "token_code too large");
    return ident_info;
  }

  iterator Find(std::string_view name) const { return hash_table_.find(name); }

  void AddKeywords();

 private:
  HashTable hash_table_;
};

}  // namespace strazer

#endif
