#pragma once

#include "token.h"

class Num: public Token {
 public:
  Num(int v)
    : Token(Tag::tNUM), value_(v) {}

  std::string toString() override;

  int value() { return value_; }

 private:
  int value_ {0};
};
