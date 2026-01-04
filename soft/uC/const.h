#pragma once

#include "expr.h"
#include "lexer.h"
#include "num.h"
#include "type.h"

#include <string>

class Constant: public Expr {
 public:
  Constant(Token* tok, Type* p)
    : Expr(tok, p) {}

  Constant(int i)
    : Expr(new Num(i), Type::Int()) {}

  static Constant* True();
  static Constant* False();

  void jumping(int t, int f) {
    if (this == True() && t != 0)
      emit("goto L" + std::to_string(t));
    else if (this == False() && f != 0)
      emit("goto L" + std::to_string(f));
  }

 private:
  static Constant* true_;
  static Constant* false_;
};
