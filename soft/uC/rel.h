#pragma once

#include "logical.h"
#include "type.h"

class Rel: public Logical {
 public:
  Rel(Token* tok, Expr* x1, Expr* x2)
    : Logical(tok, x1, x2) {}

  Type* check(Type* p1, Type* p2) {
    if (p1 instanceof Array || p2 instanceof Array)
      return nullptr;
    else if (*p1 == *p2)
      return Type::Bool();
    else
      return nullptr;
  }

  void jumping(int t, int f) {
    Expr* a = expr1.reduce();
    Expr* b = expr2.reduce();
    std::string test = a->toString() + " " +
                       op.toString() + " " + b->toString();
    emitjumps(test, t, f);
  }
};
