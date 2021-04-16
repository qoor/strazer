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

inline bool IsLiteral(TokenKind kind) {
  return tok::numeric_constant == kind || tok::char_constant == kind ||
         tok::string_literal == kind;
}

}  // namespace tok
}  // namespace strazer

#endif
