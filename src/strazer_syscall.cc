#include "strazer_syscall.h"

bool ParamList::Parse(std::string params) {
  auto front = params.front();
  auto back = params.back();
  Param param;

  if ('"' == front && '"' == back) {
    param.type = Param::ValueType::kString;
  } else if ('{' == front && '}' == back) {
    param.type = Param::ValueType::kStruct;
  } else {
    char* ptr;
    strtod(params.c_str(), &ptr);
    param.type = (ptr) ? (Param::ValueType::kInt) : (Param::ValueType::kFloat);
  }

  param.name = "";
  param.value = std::move(params);
  return true;
}

void Syscall::PushParam(std::string param) {
  if (param.empty()) return;
  printf("syscall: %s, param: %s\n", GetFunc().c_str(), param.c_str());
  params_.Parse(std::move(param));
}
