#include <iostream>

#include "regs_bank.h"
#include "spillable.h"

using namespace std;

void RegsBank0::Spill(size_t reg_idx, vector<string> &res) {
  if (pSpill_)
    pSpill_->Spill(reg_idx, res);
}

string RegsBank0::FindRegFor(string var_name, vector<string> &res) {
  static size_t last_used_register_idx {255};
  // check existing pair var:reg
  for (size_t i = 0; i < RegCnt; i++)
    if (bank0[i] == var_name) {
      last_used_register_idx = i;
      return "R" + to_string(i);
    }

  // try to insert new pair var:reg
  for (size_t i = 0; i < RegCnt; i++)
    if (!bank0[i].size()) {
      bank0[i] = var_name;
      last_used_register_idx = i;
      return "R" + to_string(i);
    }

  // no free registers, have to spill something
  if (last_used_register_idx == 255) {  // no index of last used register
    last_used_register_idx = 0;
  } else if (last_used_register_idx == 7) {
    Spill(0, res);
    last_used_register_idx = 0;
  } else {
    last_used_register_idx++;
    Spill(last_used_register_idx, res);
  }

  string reg = "R" + to_string(last_used_register_idx);
  return reg;
}

string RegsBank0::DumpRegs() {
  string res {"  // "};
  for (size_t i = 0; i < 8; i++) {
    res += "R" + to_string(i);
    res += ": ";
    res += bank0[i].size() ? bank0[i] : "-";
    if (i != 7)
      res += ", ";
  }

  return res;
}
