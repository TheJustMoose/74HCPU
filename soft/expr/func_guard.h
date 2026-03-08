#pragma once

#include <stack>
#include <string>

class FuncGuard {
 public:
  FuncGuard(std::string n) {
    call_stack_.push(n);
  }

  ~FuncGuard() {
    call_stack_.pop();
  }

  static std::string stack_str();

 private:
  static std::stack<std::string> call_stack_;
};
