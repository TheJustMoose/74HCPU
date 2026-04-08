#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class ISpillable;

class RegsBank0 {
 public:
  const size_t RegCnt {8};

  RegsBank0(ISpillable* pSpill, std::map<std::string, uint16_t>& var_addrs)
    : pSpill_(pSpill), var_addrs_(var_addrs) {}

  void Spill(size_t reg_idx, std::vector<std::string> &res);

  std::string FindRegFor(std::string var_name, std::vector<std::string> &res);
  std::string DumpRegs();

  std::string operator[](size_t idx) {
    return bank0_[idx];
  }

 private:
  ISpillable* pSpill_ {nullptr};
  std::vector<std::string> bank0_ {RegCnt};
  std::map<std::string, uint16_t>& var_addrs_;
};
