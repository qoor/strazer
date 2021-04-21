#include "strazer_identifier_table.h"

using namespace strazer;

static void AddKeyword(std::string_view keyword, tok::TokenKind token_code,
                       IdentifierTable& table) {
  table.Get(keyword, token_code);
}

void IdentifierTable::AddKeywords() {
#define KEYWORD(NAME, FLAGS) \
  AddKeyword(std::string_view(#NAME), tok::kw_##NAME, *this);
#include "strazer_token_kinds.def"
}
