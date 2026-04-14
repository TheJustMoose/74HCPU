#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class ISpillable;

class RegsBank0 {
 public:
  const size_t RegCnt {8};
  const size_t npos {static_cast<size_t>(-1)};

  RegsBank0(ISpillable* p_spill, std::map<std::string, uint16_t> var_addrs)
    : p_spill_(p_spill), var_addrs_(var_addrs) {}

  void Spill(size_t reg_idx, std::vector<std::string> &res);

  void FreeTheRegister(size_t reg_idx);

  std::string FindRegFor(std::string var_name, std::vector<std::string> &res);
  std::string DumpRegs();

  std::string operator[](size_t idx) {
    return bank0_[idx];
  }

  size_t GetIndexOfVar(std::string name);

 private:
  ISpillable* p_spill_ {nullptr};
  std::vector<std::string> bank0_ {RegCnt};
  std::map<std::string, uint16_t> var_addrs_;
};
