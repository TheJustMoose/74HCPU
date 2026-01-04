#pragma once

#include "node.h"
#include "token.h"

#include <string>

class Token;
class Type;

class Expr: public Node {
 public:
  Expr(Token* tok, Type* t)
    : op_(tok), type_(t) {}

  Expr* gen() { return this; }
  Expr* reduce() { return this; }

  void jumping(int t, int f) {
    emitjumps(toString(), t, f);
  }

  void emitjumps(std::string test, int t, int f) {
    if (t != 0 && f != 0) {
      emit("if " + test + " goto L" + std::to_string(t));
      emit("goto L" + std::to_string(f));
    }
    else if( t != 0 )
      emit("if " + test + " goto L" + std::to_string(t));
    else if( f != 0 )
      emit("iffalse " + test + " goto L" + std::to_string(f));
    else
      ; // nothing since both t and f fall through
   }

  std::string toString() { return op_->toString(); }

  Type* type() { return type_; }

 protected:
  Token* op_ {nullptr};
  Type* type_ {nullptr};
};
