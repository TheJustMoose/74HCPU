#pragma once

#include "logical.h"
#include "type.h"

class Rel: public Logical {
 public:
  Rel(Token* tok, Expr* x1, Expr* x2)
    : Logical(tok, x1, x2) {}

  Type* check(Type* p1, Type* p2) {
    if (p1->is_array() || p2->is_array())
      return nullptr;
    else if (p1 == p2)  // try to compare pointers
      return Type::Bool();
    else
      return nullptr;
  }

  void jumping(int t, int f) {
    Expr* a = expr1_->reduce();
    Expr* b = expr2_->reduce();
    std::string test = a->toString() + " " +
                       op_->toString() + " " + b->toString();
    emitjumps(test, t, f);
  }
};
