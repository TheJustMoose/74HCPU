#pragma once

#include "expr.h"

class Logical: public Expr {
 public:
  Logical(Token* tok, Expr* x1, Expr* x2)
   : Expr(tok, nullptr), expr1_(x1), expr2_(x2) {  // null type to start
    type_ = check(expr1_->type(), expr2_->type());
    if (type_ == nullptr)
      error("type error");
  }

  Type* check(Type* p1, Type* p2) {
    if (p1 == Type::Bool() && p2 == Type::Bool())
      return Type::Bool();
    else
      return nullptr;
  }

  Expr* gen() {
    int f = newlabel();
    int a = newlabel();
    Temp* temp = new Temp(type);
    jumping(0, f);
    emit(temp.toString() + " = true");
    emit("goto L" + a);
    emitlabel(f);
    emit(temp->toString() + " = false");
    emitlabel(a);
    return temp;
  }

  std::string toString() {
    return expr1_->toString() + " " + op_->toString() + " " + expr2_->toString();
  }

 public:
  Expr* expr1_ {nullptr};
  Expr* expr2_ {nullptr};
};
