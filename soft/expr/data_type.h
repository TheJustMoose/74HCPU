#pragma once

#include <string>

enum DataType {
  dtNotInitialize = 0,
  dtBool = 1,
  dtByte = 2,
  dtInt = 3,
};

std::string GetDataTypeName(DataType);
