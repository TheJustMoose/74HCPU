#include <iostream>
#include <assert.h>

#include "regs_bank.h"
#include "spillable.h"

using namespace std;

void RegsBank0::Spill(size_t reg_idx, vector<string> &res) {
  if (!p_spill_)
    return;

  assert(reg_idx < bank0_.size());

  string var_name = bank0_[reg_idx];
  assert(var_name.size());

  assert(var_addrs_.find(var_name) != var_addrs_.end());
  uint16_t var_addr = var_addrs_[var_name];

  p_spill_->Spill(reg_idx, var_addr, res);
}

string RegsBank0::FindRegFor(string var_name, vector<string> &res) {
  static size_t last_used_register_idx {255};
  // check existing pair var:reg
  for (size_t i = 0; i < RegCnt; i++)
    if (bank0_[i] == var_name) {
      last_used_register_idx = i;
      return "R" + to_string(i);
    }

  // try to insert new pair var:reg
  for (size_t i = 0; i < RegCnt; i++)
    if (!bank0_[i].size()) {
      bank0_[i] = var_name;
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
  cout << "// FindRegFor(" << var_name << "), return " << reg << endl;
  return reg;
}

string RegsBank0::DumpRegs() {
  string res {"  // "};
  for (size_t i = 0; i < 8; i++) {
    res += "R" + to_string(i);
    res += ": ";
    res += bank0_[i].size() ? bank0_[i] : "-";
    if (i != 7)
      res += ", ";
  }

  return res;
}
