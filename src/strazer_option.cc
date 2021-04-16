#include "strazer_option.h"

#include <getopt.h>

#include <cstring>

static void ShowHelp(const char* exec) {
  printf("Usage: %s [options]\n", exec);
  printf("-i, --input <path>\t\tstrace logfile path\n");
  printf("-o, --output <path>\t\tfiltered output path\n");
  printf("-c, --cups\t\toutput cups logs only\n");
}

bool Option::ParseOption(int argc, char* const argv[]) {
  static const struct option options[] = {
      {"cups", no_argument, nullptr, 'c'},
      {"help", no_argument, nullptr, 'h'},
      {"input", required_argument, nullptr, 'i'},
      {"output", required_argument, nullptr, 'o'}};

  int opt;
  int optidx;

  while (true) {
    opt = getopt_long(argc, argv, "chi:o:", options, &optidx);
    if (-1 == opt) break;
    switch (opt) {
      case 'c': {
        only_cups_ = true;
        break;
      }

      case 'h': {
        ShowHelp(argv[0]);
        return false;
      }

      case 'i': {
        if (0 >= strlen(optarg)) {
          ShowHelp(argv[0]);
          return false;
        }

        input_file_ = optarg;
        break;
      }

      case 'o': {
        if (0 >= strlen(optarg)) {
          ShowHelp(argv[0]);
          return false;
        }

        output_path_ = optarg;
        break;
      }

      default: {
        printf("unknown option: %d\n", opt);
        ShowHelp(argv[0]);
        return false;
      }
    }
  }

  return true;
}
