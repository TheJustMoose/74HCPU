#pragma once

#include "logical.h"

class And: public Logical {
 public:
  And(Token* tok, Expr* x1, Expr* x2)
   : Logical(tok, x1, x2) {}

  void jumping(int t, int f) {
    int label = f != 0 ? f : newlabel();
    expr1_->jumping(0, label);
    expr2_->jumping(t,f);
    if (f == 0)
      emitlabel(label);
  }
};
