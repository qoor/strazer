#include <cstdio>
#include <cstdlib>

#include "llvm/Support/MemoryBuffer.h"
#include "strazer_option.h"
#include "strazer_parser.h"

int main(int argc, char* argv[]) {
  Option option;
  if (!option.ParseOption(argc, argv)) return EXIT_FAILURE;

  auto file_or_err = llvm::MemoryBuffer::getFileOrSTDIN(option.GetInputFile());
  if (std::error_code ec = file_or_err.getError()) return EXIT_FAILURE;

  auto buffer = file_or_err.get()->getBuffer();
  strazer::Lexer lexer(buffer.begin(), buffer.begin(), buffer.end());
  strazer::Parser parser(lexer);
  parser.Parse();
  return EXIT_SUCCESS;
}
