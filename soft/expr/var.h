#pragma once

#include "data_type.h"

#include <cstdint>
#include <string>

class Var {
 public:
  Var(std::string n, DataType dt, bool is_p)
    : name(n), data_type(dt), is_ptr(is_p) {}

  uint8_t size() const;

  std::string name {};
  DataType data_type {dtNotInitialize};
  bool is_ptr {false};
};
