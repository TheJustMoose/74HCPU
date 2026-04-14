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
  Backend(std::map<std::string, uint16_t> var_addrs,
          std::map<size_t, std::string> idx_to_var)
    : var_addrs_(var_addrs), idx_to_var_(idx_to_var) {}

  void GenerateCode(std::vector<Operation> code);

  std::vector<std::string> GetResAsm() { return res_asm_; }

  void SwitchToBank0();
  void SwitchToBank1();

  void AddAsmInstruction(std::string instr, std::string cmnt = "", std::string cmnt2 = "");

 protected:
  void FreeTheRegisters(RegsBank0& bank0, const Operation& op);

  void GenerateAssignment(RegsBank0& bank0, Operation op, Var& v);
  void GenerateInvertion(RegsBank0& bank0, Operation op);
  void GenerateArithmOps(RegsBank0& bank0, Operation op);

 private:
  std::map<std::string, uint16_t> var_addrs_;
  std::map<size_t, std::string> idx_to_var_;
  std::vector<std::string> res_asm_;
};
