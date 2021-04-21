#include "strazer_parser.h"

#include <memory>

#include "strazer_filter.h"
#include "strazer_option.h"

static ParenType GetParenType(bool& open, char c) {
  bool tmp = open;
  open = false;
  switch (c) {
    case '(':
      open = true;
      [[fallthrough]];
    case ')':
      return ParenType::kSmall;

    case '{':
      open = true;
      [[fallthrough]];
    case '}':
      return ParenType::kMiddle;

    case '[':
      open = true;
      [[fallthrough]];
    case ']':
      return ParenType::kBig;

      // case '<':
      //   open = true;
      //   [[fallthrough]];
      // case '>':
      //   return ParenType::kArrow;

    case '"':
      open = tmp;
      return ParenType::kDoubleQuote;
  }

  return ParenType::kInvalid;
}

static bool IsNumeric(char c) { return isdigit(c); }
static bool IsWhitespace(char c) { return isspace(c); }
static bool IsIdentStart(char c) { return isalpha(c); }
static bool IsIdentContinue(char c) { return (c == '_' || isalnum(c)); }

bool Parser::Parse() {
  const auto& input_file = option_.GetInputFile();
  if (!input_file.empty()) return ParseFromFilepath(input_file);
  return ParseFromFile(stdin);
}

bool Parser::ParseFromFilepath(const std::string& log_path) {
  class SafeFile {
   public:
    ~SafeFile() {
      if (ptr) {
        fclose(ptr);
        ptr = nullptr;
      }
    }
    bool Open(const std::string& path) {
      FILE* f = fopen(path.c_str(), "r");
      if (!f) return false;
      ptr = f;
      return true;
    }

    FILE* ptr = nullptr;
  };
  SafeFile file;

  if (!file.Open(log_path)) {
    printf("Failed to open input file: %s\n", log_path.c_str());
    return false;
  }

  return ParseFromFile(file.ptr);
}

bool Parser::ParseFromFile(FILE* file) {
  if (!file) return false;

  const size_t size = 4096 * 10 * 10;
  std::unique_ptr<char[]> ptr(new char[size]);
  if (!ptr) return false;

  int line = 0;
  Filter filter(option_);
  while (!feof(file)) {
    if (!fgets(ptr.get(), size, file)) break;
    ++line;

    if (!ParseLine(ptr.get())) {
      printf("Line %d parse failed.\n", line);
      break;
    }

    if (filter.CanPrint(cur_pid_->GetSyscall())) {
      // printf("%s", ptr.get());
    }
  }

  return true;
}

bool Parser::ParseLine(std::string line) {
  if (line.empty()) return true;

  log_.Init(std::move(line));
  if (!ParsePid() || !ParseCall()) return false;

  return true;
}

bool Parser::ParsePid() {
  std::string pid_name;
  pid_name = log_.EatWhile(IsNumeric);

  if (!pid_name.empty()) {
    pid_t pid = std::stoi(pid_name);
    if (!pid_root_.GetPid()) {
      pid_root_.SetPid(pid);
    } else {
      auto* pid_node = pid_root_.FindPid(pid);
      if (!pid_node) pid_node = &pid_root_.Childbirth(pid, false);
      cur_pid_ = pid_node;
    }
  }

  return true;
}

bool Parser::ParseCall() {
  std::string syscall_name;
  std::string ret_value;
  auto& syscall = cur_pid_->GetSyscall();

  syscall.Reset();
  SkipWhitespace();

  if ('+' == log_.First()) {
    if (ProcExitType::kInvalid == ProcExit(ret_value)) return false;
  }

  if (syscall.IsUnfinished()) {
    if (!log_.BumpIfMatch("<... ")) {
      printf("pid %d syscall %s required resume, but not found.\n",
             cur_pid_->GetPid(), syscall.GetFunc().c_str());
      return false;
    }

    if (!IsIdentStart(log_.First()) || !log_.BumpIfMatch(" resumed>")) {
      printf("syscall resume log is not correct.\n");
      return false;
    }

    syscall.SetUnfinished(false);
  } else if (!syscall.IsUnfinished()) {
    if (IsIdentStart(log_.First()))
      syscall_name = log_.EatWhile(IsIdentContinue);

    if ('(' != log_.Bump()) return true;
    syscall.SetFunc(syscall_name);
  }

  if (!ParseParams()) return false;

  if (!syscall.IsUnfinished()) {
    SkipWhitespace();
    if (!log_.BumpIfMatch("= ")) {
      printf("syscall %s does not have return value.\n",
             syscall.GetFunc().c_str());
      return false;
    }

    syscall.SetReturnValue(log_.EatWhile(IsIdentContinue));
  }

  return true;
}

bool Parser::ParseParams() {
  auto& syscall = cur_pid_->GetSyscall();
  auto& parened = cur_pid_->GetParened();
  ParenType paren;
  bool paren_open;
  bool in_string = false;
  char c;
  std::string param;
  int escape = 0;
  int escape_chars = 0;

  SkipWhitespace();

  while ((c = log_.Bump())) {
    if (in_string) {
      if ('\\' == c && (2 != escape || escape_chars)) {
        escape = 1;
        escape_chars = 0;
      }

      if (2 == escape) {
        if (isdigit(c)) {
          if (3 < ++escape_chars) {
            escape = 0;
            escape_chars = 0;
          }
        } else if (!escape_chars) {
          escape = 3;
        } else {
          escape = 0;
        }
      } else if (3 == escape) {
        escape = 0;
      }
    }

    if ('<' == c && log_.Match("unfinished ...>")) {
      syscall.SetUnfinished(true);
      return true;
    }

    if (!escape) {
      paren = GetParenType(paren_open, c);
      if (ParenType::kInvalid != paren) {
        if (ParenType::kDoubleQuote == paren) {
          in_string = !in_string;
          continue;
        }

        if (!in_string) {
          if (paren_open) {
            parened.emplace(paren);
          } else {
            if (parened.empty()) {
              if (ParenType::kSmall != paren) {
                printf("expected \")\", but found \"%c\"", c);
                return false;
              }

              syscall.PushParam(param);
              return true;
            }

            if (parened.top() != paren) {
              printf("incorrect parenthese pair.\n");
              break;
            }

            parened.pop();
          }
        }
      }
    }

    if (parened.empty()) {
      if (!in_string && ',' == c) {
        syscall.PushParam(param);
        param.clear();
        SkipWhitespace();
      } else {
        param += c;
      }
    }

    if (escape == 1) escape = 2;
  }

  if (in_string) {
    printf("string is not ended.\n");
  }

  while (!parened.empty()) {
    printf("Paren %d not closed.\n", static_cast<int>(parened.top()));
    parened.pop();
  }

  return false;
}

void Parser::SkipWhitespace() { log_.EatWhile(IsWhitespace); }

Parser::ProcExitType Parser::ProcExit(std::string& code) {
  if (!log_.BumpIfMatch("+++ ")) return ProcExitType::kInvalid;

  ProcExitType reason = ProcExitType::kInvalid;
  if (log_.BumpIfMatch("exited with ")) {
    reason = ProcExitType::kExited;
  } else if (log_.BumpIfMatch("killed by ")) {
    reason = ProcExitType::kKilled;
  } else {
    printf("process exit log is not correct.\n");
    return ProcExitType::kInvalid;
  }

  auto s = log_.EatWhile(IsIdentContinue);
  printf("Exit code: %s\n", s.c_str());
  if (s.empty() || !log_.Match(" +++")) {
    printf("process exit log is not correct.\n");
    return ProcExitType::kInvalid;
  }

  code = s;
  pid_root_.DropChild(cur_pid_);
  return reason;
}
