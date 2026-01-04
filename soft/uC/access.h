#pragma once

#include "id.h"
#include "op.h"
#include "tag.h"

class Access: public Op {
 public:
  Id* array_ {nullptr};
  Expr* index_ {nullptr};

  Access(Id* a, Expr* i, Type* p)       // p is element type after
    : Op(new Word("[]", Tag::tINDEX), p), // flattening the array
    array_(a), index_(i) {}

  Expr* gen() {
    return new Access(array_, index_->reduce(), type_);
  }

  void jumping(int t,int f) {
    emitjumps(reduce()->toString(), t, f);
  }

  std::string toString() {
    return array_->toString() + " [ " + index_->toString() + " ]";
  }
};
