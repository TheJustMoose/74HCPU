#pragma once

#include "expr.h"

#include <string>

class Type;

class Temp: public Expr {
 public:
  Temp(Type* p);

  std::string toString() {
    return "t" + std::to_string(number_);
  }

 private:
  static int count_;
  int number_ {0};
};
