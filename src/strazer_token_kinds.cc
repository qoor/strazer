#include "strazer_token_kinds.h"

#include <cstdio>

using namespace strazer;

static const char* const gs_kTokNames[] = {
#define TOK(X) #X,
#define KEYWORD(X, Y) #X,
#include "strazer_token_kinds.def"
    nullptr};

const char* tok::GetTokenName(TokenKind kind) {
  if (tok::kNumTokens > kind) return gs_kTokNames[kind];
  printf("unknown TokenKind\n");
  return nullptr;
}

const char* tok::GetPunctuatorSpelling(TokenKind kind) {
  switch (kind) {
#define PUNCTUATOR(X, Y) \
  case X:                \
    return Y;
#include "strazer_token_kinds.def"
    default:
      break;
  }

  return nullptr;
}

const char* tok::GetKeywordSpelling(TokenKind kind) {
  switch (kind) {
#define KEYWORD(X, Y) \
  case kw_##X:        \
    return #X;
#include "strazer_token_kinds.def"
    default:
      break;
  }

  return nullptr;
}
