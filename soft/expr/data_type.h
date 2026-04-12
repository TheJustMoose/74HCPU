#pragma once

#include <string>

enum DataType {
  dtNotInitialize = 0,
  dtByte = 1,
  dtInt = 2,
};

std::string GetDataTypeName(DataType);
