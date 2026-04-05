#pragma once

#include "operation.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class RegsBank0;
class Var;

class Backend {
 public:
  Backend(std::map<std::string, uint16_t> var_addrs)
    : var_addrs_(var_addrs) {}

  void GenerateCode(std::vector<Operation> code);

  std::vector<std::string> GetResAsm() {
    return res_asm_;
  }

 protected:
  void GenerateAssignment(RegsBank0& bank0, Operation op, Var& v);
  void GenerateInvertion(RegsBank0& bank0, Operation op);

 private:
  std::map<std::string, uint16_t> var_addrs_;
  std::vector<std::string> res_asm_;
};
