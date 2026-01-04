#pragma once

#include "logical.h"

class Or: public Logical {
 public:
  Or(Token* tok, Expr* x1, Expr* x2)
    : Logical(tok, x1, x2) {}

  void jumping(int t, int f) {
    int label = t != 0 ? t : newlabel();
    expr1_->jumping(label, 0);
    expr2_->jumping(t, f);
    if (t == 0)
      emitlabel(label);
  }
};
