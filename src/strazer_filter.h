#ifndef _STRAZER_FILTER_H
#define _STRAZER_FILTER_H

#include <vector>

class Option;
class Syscall;

class Filter {
 public:
  Filter(const Option& option) : option_(option) {}
  bool CanPrint(const Syscall& syscall);

 private:
  const Option& option_;

  std::vector<int> cups_fds_;
};

#endif
