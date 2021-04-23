#ifndef _STRAZER_AST_H
#define _STRAZER_AST_H

#include <cctype>
#include <memory>
#include <vector>

namespace strazer {

class IdentifierInfo;

class Expr {
 public:
  enum ExprClass {
    kExprClass,
    kIntegerLiteralClass,
    kCharacterLiteralClass,
    kFloatingLiteralClass,
    kStringLiteralClass,
    kArrayExprClass,
    kStructExprClass,
    kPidClass,
    kSyscallClass,
    kCallExprClass
  };

  static bool classof(const Expr* expr) {
    return kExprClass == expr->GetExprClass();
  }

  ExprClass GetExprClass() const { return expr_class_; }

 private:
  ExprClass expr_class_ = kExprClass;
};

class VarDecl {
 public:
  VarDecl(IdentifierInfo* id, Expr value) : id_(id), value_(value) {}
  VarDecl() = default;

 private:
  IdentifierInfo* id_ = nullptr;
  Expr value_;
};

class IntegerLiteral : public Expr {
 public:
  static bool classof(const Expr* expr) {
    return kIntegerLiteralClass == expr->GetExprClass();
  }

  explicit IntegerLiteral(uint64_t value) : value_(value) {}
  IntegerLiteral() = default;

 private:
  uint64_t value_;
};

class CharacterLiteral : public Expr {
 public:
  static bool classof(const Expr* expr) {
    return kCharacterLiteralClass == expr->GetExprClass();
  }

  explicit CharacterLiteral(char value) : value_(value) {}
  CharacterLiteral() = default;

 private:
  char value_;
};

class FloatingLiteral : public Expr {
 public:
  static bool classof(const Expr* expr) {
    return kFloatingLiteralClass == expr->GetExprClass();
  }

  explicit FloatingLiteral(double value) : value_(value) {}
  FloatingLiteral() = default;

 private:
  double value_;
};

class StringLiteral : public Expr {
 public:
  static bool classof(const Expr* expr) {
    return kStringLiteralClass == expr->GetExprClass();
  }

  explicit StringLiteral(std::string_view value) : value_(value) {}
  StringLiteral() = default;

 private:
  std::string_view value_;
};

class ArrayExpr : public Expr {
 public:
  static bool classof(const Expr* expr) {
    return kArrayExprClass == expr->GetExprClass();
  }

  explicit ArrayExpr(std::vector<Expr> values) : values_(std::move(values)) {}
  ArrayExpr() = default;

 private:
  std::vector<Expr> values_;
};

class StructExpr : public Expr {
 public:
  static bool classof(const Expr* expr) {
    return kStructExprClass == expr->GetExprClass();
  }

  explicit StructExpr(std::vector<Expr> members)
      : members_(std::move(members)) {}
  StructExpr() = default;

 private:
  std::vector<Expr> members_;
};

class Pid : public Expr {};

class Function : public Expr {
 private:
  std::vector<Expr> args_;
};

class CallExpr : public Expr {
 public:
 private:
  std::unique_ptr<Pid> pid_;
  std::unique_ptr<Function> syscall_;
};

}  // namespace strazer

#endif
