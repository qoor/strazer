#ifndef _STRAZER_TOKEN_KINDS_H
#define _STRAZER_TOKEN_KINDS_H

namespace strazer {
namespace tok {

enum TokenKind {
#define TOK(x) x,
#include "strazer_token_kinds.def"
  kNumTokens
};

const char* GetTokenName(TokenKind kind);
const char* GetPunctuatorSpelling(TokenKind kind);
const char* GetKeywordSpelling(TokenKind kind);

inline bool IsAnyIdentifier(TokenKind kind) {
  return tok::identifier == kind || tok::raw_identifier == kind;
}

inline bool IsStringLiteral(TokenKind kind) {
  return tok::string_literal == kind;
}

inline bool IsLiteral(TokenKind kind) {
  return tok::numeric_constant == kind || tok::char_constant == kind ||
         IsStringLiteral(kind);
}

}  // namespace tok
}  // namespace strazer

#endif
