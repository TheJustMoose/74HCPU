#pragma once

#include "op.h"

class Unary: public Op {
 public:
  Expr* expr_ { nullptr };

  Unary(Token* tok, Expr* x)
    : Op(tok, nullptr), expr_(x) {  // handles minus, for ! see Not
    type_ = Type::max(Type::Int(), expr_->type());
    if (type_ == nullptr)
      error("type error");
   }

  Expr* gen() {
    return new Unary(op_, expr->reduce());
  }

  std::string toString() {
    return op_->toString() + " " + expr->toString();
  }
};
