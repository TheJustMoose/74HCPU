#pragma once

#include "operation.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

std::vector<std::string> GenerateCode(std::vector<Operation> code,
                                      std::map<std::string, uint16_t> var_addrs);
