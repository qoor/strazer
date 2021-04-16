#include "strazer_filter.h"

#include "strazer_option.h"
#include "strazer_parser.h"

bool Filter::CanPrint(const Syscall& syscall) {
  const auto& func = syscall.GetFunc();
  if (func.empty()) return true;
  return true;
}
