#pragma once

#include <string>

#include "access.h"
#include "expr.h"
#include "id.h"
#include "lexer.h"
#include "node.h"
#include "type.h"
#include "word.h"

// TODO: try to implement it:
// std::string operator+(const char*, int);

class Stmt: public Node {
 public:
  Stmt() {}

  static Stmt* Null() {
    return new Stmt();  // TODO: should it be singleton?
  }

  void gen(int b, int a) {} // called with labels begin and after
  int after() { return after_; }

  static Stmt* Enclosing;    // used for break stmts

 protected:
  int after_ {0};           // saves label after
};

class If: public Stmt {
 public:
  If(Expr* x, Stmt* s)
    : expr_(x), stmt_(s) {
    if (expr_->type() != Lexer::get_word("bool"))
      error("boolean required in if");
  }

  void gen(int b, int a) {
    int label = newlabel();  // label for the code for stmt
    expr_->jumping(0, a);    // fall through on true, goto a on false
    emitlabel(label);
    stmt_->gen(label, a);
  }

 private:
  Expr* expr_ {nullptr};
  Stmt* stmt_ {nullptr};
};

class Else: public Stmt {
 public:
  Else(Expr* x, Stmt* s1, Stmt* s2)
    : expr_(x), stmt1_(s1), stmt2_(s2) {
    if (expr_->type() != Type::Bool())
      expr_->error("boolean required in if");
  }

  void gen(int b, int a) {
    int label1 = newlabel();    // label1 for stmt1
    int label2 = newlabel();    // label2 for stmt2
    expr_->jumping(0,label2);   // fall through to stmt1 on true
    emitlabel(label1);
    stmt1_->gen(label1, a);
    emit("goto L" + std::to_string(a));
    emitlabel(label2);
    stmt2_->gen(label2, a);
  }

 private:
  Expr* expr_ {nullptr};
  Stmt* stmt1_ {nullptr};
  Stmt* stmt2_ {nullptr};
};

class Do: public Stmt {
 public:
  Do() {}

  void init(Stmt* s, Expr* x) {
    expr_ = x;
    stmt_ = s;
    if (expr_->type() != Type::Bool())
       expr_->error("boolean required in do");
  }

 public:
  Expr* expr_ {nullptr};
  Stmt* stmt_ {nullptr};
};

class While: public Stmt {
 public:
  While() {}

  void init(Expr* x, Stmt* s) {
    expr_ = x;
    stmt_ = s;
    if (expr_->type() != Type::Bool())
      expr_->error("boolean required in while");
  }

  void gen(int b, int a) {
    after_ = a;               // save label a
    expr_->jumping(0, a);
    int label = newlabel();   // label for stmt
    emitlabel(label);
    stmt_->gen(label, b);
    emit("goto L" + std::to_string(b));
  }

 public:
  Expr* expr_ {nullptr};
  Stmt* stmt_ {nullptr};
};

class Break: public Stmt {
 public:
  Break() {
    if (Stmt::Enclosing == Stmt::Null())
      error("unenclosed break");
    stmt_ = Stmt::Enclosing;
  }

  void gen(int b, int a) {
    emit("goto L" + std::to_string(stmt_->after()));
  }

 private:
  Stmt* stmt_ {nullptr};
};

class Set: public Stmt {
 public:
  Set(Id* id, Expr* x) {}
};

class SetElem: public Stmt {
 public:
  SetElem(Access* x, Expr* y) {}
};
