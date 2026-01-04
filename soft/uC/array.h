#pragma once

#include "tag.h"
#include "type.h"

#include <string>

class Array: public Type {
 public:
  Type* of_ {nullptr};    // array *of* type
  int size_ {1};          // number of elements

  Array(int sz, Type* p)
    : Type("[]", Tag::tINDEX, sz*p->width()), size_(sz), of_(p) {}

  std::string toString() {
    return std::string("[") + std::to_string(size_) +
           std::string("] ") + of_->toString();
  }
};
