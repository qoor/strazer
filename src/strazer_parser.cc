#include "strazer_parser.h"

using namespace strazer;

void Parser::Lex(Token& result) {
  bool returned_token;
  do {
    returned_token = lexer_.Lex(result);
  } while (!returned_token);

  if (result.IsKind(tok::unknown)) return;
}

void Parser::Parse() {
  Token tok;

  while (true) {
    Lex(tok);
    tok.Print();
    switch (tok.GetKind()) {
      case tok::numeric_constant: {
        ParsePID();
        break;
      }

      case tok::unknown: {
        printf("Unknown symbol\n");
        break;
      }

      default: {
        break;
      }
    }

    if (tok.IsKind(tok::eof)) break;

    tok_ = tok;
  }
}

void Parser::ParsePID() {}

Token Parser::PeekAhead(size_t n) {
  Token tok;
  for (; n > 0; --n) {
    Lex(tok);
  }
  return tok;
}
