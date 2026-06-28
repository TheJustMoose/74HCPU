#pragma once

#include <string>

enum DataType {
  dtNotInitialize = 0,
  dtNotApplicable = 1,
  dtBool = 2,
  dtByte = 3,
  dtInt = 4
};

std::string GetDataTypeName(DataType);
