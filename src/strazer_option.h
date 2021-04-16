#ifndef _STRAZER_OPTION_H
#define _STRAZER_OPTION_H

#include <string>

class Option {
 public:
  bool ParseOption(int argc, char* const argv[]);

  std::string GetInputFile() const { return input_file_; }
  std::string GetOutputPath() const { return output_path_; }
  bool IsOnlyCups() const { return only_cups_; }

 private:
  std::string input_file_;
  std::string output_path_;
  bool only_cups_ = false;
};

#endif
