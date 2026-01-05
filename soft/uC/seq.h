#pragma once

#include "stmt.h"

class Seq: public Stmt {
 public:
  Seq(Stmt* s1, Stmt* s2)
    : stmt1_(s1), stmt2_(s2) {}

  void gen(int b, int a) {
    if (stmt1_ == Stmt::Null())
      stmt2_->gen(b, a);
    else if (stmt2_ == Stmt::Null())
      stmt1_->gen(b, a);
    else {
       int label = newlabel();
       stmt1_->gen(b, label);
       emitlabel(label);
       stmt2_->gen(label, a);
    }
  }

 public:
  Stmt* stmt1_ {nullptr};
  Stmt* stmt2_ {nullptr};
};
