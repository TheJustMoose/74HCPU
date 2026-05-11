#pragma once

#include <cstdint>
#include <string>

#include "data_type.h"
#include "var.h"

size_t getVarCount();
Var getVar(size_t idx);
bool isDeclared(std::string var_name, uint8_t* var_size = 0, DataType* dt = 0);
void printVars();
bool getVar(std::string var_name, Var& var);
