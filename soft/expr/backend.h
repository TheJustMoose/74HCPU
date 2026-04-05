#pragma once

#include "operation.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class Backend {
 public:
  Backend(std::map<std::string, uint16_t> var_addrs)
    : var_addrs_(var_addrs) {}

  std::vector<std::string> GenerateCode(std::vector<Operation> code);

 private:
  std::map<std::string, uint16_t> var_addrs_;
};
