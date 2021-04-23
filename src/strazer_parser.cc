#include "strazer_parser.h"

using namespace strazer;

void Parser::Lex(Token& result) {
  bool returned_token;
  do {
    returned_token = lexer_.Lex(result);
  } while (!returned_token);

  if (result.IsKind(tok::unknown)) return;
}

void Parser::Initialize() { ConsumeToken(); }

bool Parser::Parse() {
  assert(kStateNone == state_);
  switch (tok_.GetKind()) {
    case tok::eof: {
      return true;
    }
    case tok::numeric_constant: {
      ParsePid();
      return false;
    }
    case tok::identifier: {
      ParseSyscall();
      return false;
    }
    default: {
      return true;
    }
  }
}

bool Parser::ParsePid() { ParseSyscall(); }

Token Parser::PeekAhead(size_t n) {
  Token tok;
  for (; n > 0; --n) {
    Lex(tok);
  }
  return tok;
}

void ParseAST(Lexer& lexer) {
  auto parse_op = std::make_unique<Parser>(lexer);
  if (!parse_op) {
    printf("Failed to create parser instance.\n");
    return;
  }

  auto& p = *parse_op.get();
  p.Initialize();
  for (bool at_eof = p.Parse(); !at_eof; at_eof = p.Parse()) {
    continue;
  }
}
