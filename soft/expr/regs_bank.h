#pragma once

#include <cstring>
#include <string>
#include <vector>

#include "spill.h"

class RegsBank0: public ISpillable {
 public:
  const size_t RegCnt {8};

  void Spill(size_t reg_idx, /*string some_var_name,*/ std::vector<std::string> &res) override;

  std::string FindRegFor(std::string var_name, std::vector<std::string> &res);
  std::string DumpRegs();

  std::string operator[](size_t idx) {
    return bank0[idx];
  }

 private:
  std::vector<std::string> bank0 {RegCnt};
};
