#ifndef _STRAZER_CURSOR_H
#define _STRAZER_CURSOR_H

#include <cstring>
#include <iterator>
#include <string_view>

namespace strazer {

class Cursor {
 public:
  Cursor(const char* str) : str_(str), initial_len_(strlen(str)) {}
  Cursor(const char* str, size_t len) : str_(str, len), initial_len_(len) {}
  Cursor(const std::string& str) : str_(str), initial_len_(str_.size()) {}

  // Allow copy
  Cursor(const Cursor& src) = default;
  Cursor operator=(const Cursor& src) { return Cursor(src); }

  char First() const { return NthChar(0); }
  char Second() const { return NthChar(1); }
  bool IsEof() const { return str_.empty(); }
  size_t LenConsumed() const { return initial_len_ - str_.size(); }
  std::string String() const { return std::string(str_); }
  char Bump() {
    char c = First();
    if (c) str_.remove_prefix(1);
    return c;
  }

  std::string EatWhile(bool (*predicate)(char)) {
    std::string out;
    while (predicate(First()) && !IsEof()) out += Bump();
    return out;
  }

 private:
  char NthChar(size_t n) const {
    if (str_.size() <= n) return '\0';
    return str_[n];
  }

  std::string_view str_;
  size_t initial_len_;
};

}  // namespace strazer

#endif
