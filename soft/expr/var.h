#pragma once

#include "data_type.h"

#include <string>

struct Var {
  std::string name {};
  DataType data_type {dtNotInitialize};

  Var(std::string n, DataType dt)
    : name(n), data_type(dt) {}

  uint8_t size() const;
};
