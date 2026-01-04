#pragma once

#include "expr.h"
#include "lexer.h"
#include "num.h"
#include "type.h"

class Constant: public Expr {
 public:
  Constant(Token* tok, Type* p)
    : Expr(tok, p) {}

  Constant(int i)
    : Expr(new Num(i), Type::Int()) {}

  static Constant* True() {
    if (!true_)
      true_ = new Constant(Lexer::get_word("true"),  Type::Bool());
    return true_;
  }

  static Constant* False() {
    if (!false_)
      false_ = new Constant(Lexer::get_word("false"),  Type::Bool());
    return false_;
  }

  void jumping(int t, int f) {
    if (this == True && t != 0)
      emit("goto L" + t);
    else if (this == False && f != 0)
      emit("goto L" + f);
  }

 private:
  static Constant* true_;
  static Constant* false_;
};
