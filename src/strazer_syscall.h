#ifndef _STRAZER_PARAM_H
#define _STRAZER_PARAM_H

#include <list>
#include <string>
#include <vector>

struct Param {
  enum class ValueType { kUnknown, kInt, kFloat, kString, kArray, kStruct };

  bool IsName(const std::string& name) const {
    return (!this->name.compare(name));
  }

  ValueType type = ValueType::kUnknown;
  Param* parent;
  std::string name;
  std::string value;
  std::list<Param> childs;
};

struct ParamList {
 public:
  void Reset() { params_.clear(); }
  bool Parse(std::string params);

  const Param* GetParam(size_t index) const {
    if (params_.size() > index) return &params_[index];
    return nullptr;
  }

  size_t GetNumParams() const { return params_.size(); }

 private:
  std::vector<Param> params_;
};

class Syscall {
 public:
  void Reset() {
    func_.clear();
    params_.Reset();
    ret_.clear();

    unfinished_ = false;
  }

  void PushParam(std::string param);

  bool IsUnfinished() const { return unfinished_; }
  void SetUnfinished(bool unfinished) { unfinished_ = unfinished; }

  const std::string& GetFunc() const { return func_; }
  void SetFunc(std::string func) { func_ = std::move(func); }

  std::string GetReturnValue() const { return ret_; }
  void SetReturnValue(std::string ret) { ret_ = std::move(ret); }

  const ParamList& GetParams() const { return params_; }

 private:
  std::string func_;
  ParamList params_;
  std::string ret_;

  bool unfinished_ = false;
};

#endif
