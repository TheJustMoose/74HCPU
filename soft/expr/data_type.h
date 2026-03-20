#pragma once

#include <string>

enum DataType {
  dtNotInitialize,
  dtByte,
  dtInt,
};

std::string GetDataTypeName(DataType);
