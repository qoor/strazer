#ifndef _STRAZER_PREPROCESSOR_H
#define _STRAZER_PREPROCESSOR_H

#include <cassert>
#include <memory>
#include <vector>

#include "strazer_identifier_table.h"
#include "strazer_lexer.h"
#include "strazer_token.h"
#include "strazer_token_kinds.h"
#include "strazer_utils.h"

namespace strazer {

class Parser {
 public:
  Parser(std::string_view str)
      : lexer_(str.data(), str.data(), str.data() + str.size()) {}
  Parser(Lexer& lexer) : lexer_(lexer) {}

  const Token& GetCurToken() const { return tok_; }

  void Lex(Token& result);
  void Parse();

 private:
  bool IsTokenParen() const {
    return tok_.IsOneOfKind(tok::l_paren, tok::r_paren);
  }
  bool IsTokenBracket() const {
    return tok_.IsOneOfKind(tok::l_square, tok::r_square);
  }
  bool IsTokenBrace() const {
    return tok_.IsOneOfKind(tok::l_brace, tok::r_brace);
  }
  bool IsTokenStringLiteral() const { return tok_.IsKind(tok::string_literal); }
  bool IsTokenSpecial() const {
    return IsTokenStringLiteral() || IsTokenParen() || IsTokenBracket() ||
           IsTokenBrace();
  }

  void ConsumeParen() {
    assert(IsTokenParen() && "wrong consume method");
    if (tok::l_paren == tok_.GetKind())
      ++parens_;
    else if (parens_)
      --parens_;
    Lex(tok_);
  }

  void ConsumeBracket() {
    assert(IsTokenBracket() && "wrong consume method");
    if (tok::l_square == tok_.GetKind())
      ++brackets_;
    else if (brackets_)
      --brackets_;
    Lex(tok_);
  }

  void ConsumeBrace() {
    assert(IsTokenBrace() && "wrong consume method");
    if (tok::l_brace == tok_.GetKind())
      ++braces_;
    else if (braces_)
      --braces_;
    Lex(tok_);
  }

  void ConsumeStringToken() {
    assert(IsTokenStringLiteral() &&
           "Should only consume string literals with this method");
    Lex(tok_);
  }

  void CutOffParsing() { tok_.SetKind(tok::eof); }

  bool IsEof() const { return tok::eof == tok_.GetKind(); }

  Token GetLookAheadToken(size_t n) {
    if (0 == n || tok_.IsKind(tok::eof)) return tok_;
    return LookAhead(n - 1);
  }

  Token LookAhead(size_t n) { return PeekAhead(n + 1); }

  Token PeekAhead(size_t n);

  void ParsePID();

  Lexer lexer_;

  Token tok_;

  unsigned short parens_ = 0;
  unsigned short brackets_ = 0;
  unsigned short braces_ = 0;
};

}  // namespace strazer

#endif
