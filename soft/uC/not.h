#pragma once

#include "logical.h"

class Not: public Logical {
 public:
  Not(Token* tok, Expr* x2)
   : Logical(tok, x2, x2) {}

  void jumping(int t, int f) {
    expr2_->jumping(f, t);
  }

  std::string toString() {
    return op_->toString() + " " + expr2_->toString();
  }
};
