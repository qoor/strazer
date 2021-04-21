#ifndef _STRAZER_PARSER_H
#define _STRAZER_PARSER_H

#include <list>
#include <stack>
#include <string>
#include <vector>

#include "strazer_cursor.h"
#include "strazer_syscall.h"

enum class ParenType { kInvalid, kSmall, kMiddle, kBig, kArrow, kDoubleQuote };

class PidNode {
 public:
  PidNode() = default;
  PidNode(PidNode* parent, pid_t pid, bool forked)
      : pid_(pid), forked_(forked), parent_(parent) {}

  PidNode& Childbirth(pid_t pid, bool fork) {
    childs_.emplace_back(this, pid, fork);
    return childs_.back();
  }
  bool DropChild(PidNode* child) {
    if (!child) return false;
    if (this == child) return true;

    auto* parent = child->GetParent();
    if (!parent) return false;
    if (this == parent) return true;

    parent->childs_.remove(*child);
    return true;
  }

  PidNode* GetParent() { return parent_; }
  PidNode* GetParent(int level) {
    PidNode* p = nullptr;
    for (p = this; p && 0 < level; --level, p = p->parent_) continue;
    return parent_;
  }

  PidNode* FindPid(pid_t pid) {
    if (!pid) return nullptr;
    if (pid_ == pid) return this;

    PidNode* node;
    for (auto& child : childs_) {
      if ((node = child.FindPid(pid))) return node;
    }

    return nullptr;
  }

  pid_t GetPid() const { return pid_; }
  void SetPid(pid_t pid) {
    if (0 == pid_ && childs_.empty()) pid_ = pid;
  }

  bool IsForked() const { return forked_; }

  Syscall& GetSyscall() { return syscall_; }
  std::stack<ParenType>& GetParened() { return parened_; }

  bool operator==(const PidNode& rhs) const { return (pid_ == rhs.pid_); }

 private:
  pid_t pid_ = 0;
  bool forked_ = false;
  Syscall syscall_;
  std::stack<ParenType> parened_;

  PidNode* parent_ = nullptr;
  std::list<PidNode> childs_;
};

class Option;
class Parser {
 public:
  Parser(Option& option) : option_(option) {}

  bool Parse();

  const Option& GetOption() const { return option_; }
  const PidNode& GetPidRoot() const { return pid_root_; }

 private:
  enum class LogState { kNormal, kComment };
  enum class ProcExitType { kInvalid, kExited, kKilled };

  bool ParseFromFilepath(const std::string& log_path);
  bool ParseFromFile(FILE* file);
  bool ParseLine(std::string line);
  bool ParsePid();
  bool ParseCall();
  bool ParseParams();
  void SkipWhitespace();

  ProcExitType ProcExit(std::string& code);

  Option& option_;

  PidNode pid_root_;
  PidNode* cur_pid_ = &pid_root_;

  Cursor log_;

  LogState state_ = LogState::kNormal;
};

#endif
