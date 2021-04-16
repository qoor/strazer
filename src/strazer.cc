#include <cstdio>
#include <cstdlib>

#include "strazer_option.h"
#include "strazer_parser.h"

int main(int argc, char* argv[]) {
  Option option;
  if (!option.ParseOption(argc, argv)) return EXIT_FAILURE;

  Parser parser(option);
  if (!parser.Parse()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
