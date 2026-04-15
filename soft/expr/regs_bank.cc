#include <iostream>
#include <assert.h>

#include "regs_bank.h"
#include "spillable.h"

using namespace std;

void RegsBank0::Spill(size_t reg_idx) {
  if (!p_spill_)
    return;

  assert(reg_idx < bank0_.size());

  string var_name = bank0_[reg_idx];
  assert(var_name.size());
  assert(var_addrs_.find(var_name) != var_addrs_.end());
  uint16_t var_addr = var_addrs_[var_name];

  cout << DumpRegs() << endl;
  p_spill_->Spill(reg_idx, var_addr, var_name);
  var_was_spilled_[var_name] = true;
  bank0_[reg_idx] = "";  // mark register as free
}

void RegsBank0::Fill(string var_name, string reg_name) {
  if (var_was_spilled_.find(var_name) == var_was_spilled_.end() ||
      !var_was_spilled_[var_name])
    return;

  p_spill_->Fill(var_name, reg_name);
  var_was_spilled_[var_name] = false;
}

void RegsBank0::FreeTheRegister(size_t reg_idx) {
  bank0_[reg_idx] = "";  // mark register as free
  cout << "Register " << reg_idx << " was released" << endl;
}

string RegsBank0::FindRegFor(string var_name) {
  cout << "FindRegFor: " << var_name << endl;
  static size_t last_used_register_idx {255};
  // check existing pair var:reg
  for (size_t i = 0; i < RegCnt; i++)
    if (bank0_[i] == var_name) {
      last_used_register_idx = i;
      cout << "return existent R" << i << " for " << var_name << endl;
      return "R" + to_string(i);
    }

  // try to insert new pair var:reg
  for (size_t i = 0; i < RegCnt; i++)
    if (!bank0_[i].size()) {
      bank0_[i] = var_name;
      last_used_register_idx = i;
      string reg_name = "R" + to_string(i);
      cout << "return empty " << reg_name << " for " << var_name << endl;
      Fill(var_name, reg_name);  // probably I should read var from memory
      return reg_name;           // but what if register was really empty? Oo
    }

  // no free registers, have to spill something
  if (last_used_register_idx == 255) {  // no index of last used register
    last_used_register_idx = 0;
    cout << "Why I am here????!!!" << endl;
    throw "Error happened. last_used_register_idx is empty but should be initialized!";
  } else if (last_used_register_idx == 7) {
    last_used_register_idx = 0;                    // remember!
    Spill(last_used_register_idx);                 // free
    bank0_[last_used_register_idx] = var_name;     // occupy
    cout << "Spill and return R0" << " for " << var_name << endl;
  } else {
    last_used_register_idx++;  // now "last" used register must be different
    Spill(last_used_register_idx);                 // free
    bank0_[last_used_register_idx] = var_name;     // occupy
    cout << "Spill and return R" << last_used_register_idx << " for " << var_name << endl;
  }

  string reg_name = "R" + to_string(last_used_register_idx);
  Fill(var_name, reg_name);  // probably I should read var from memory
  return reg_name;
}

string RegsBank0::DumpRegs() {
  string res {"// "};
  for (size_t i = 0; i < 8; i++) {
    res += "R" + to_string(i);
    res += ": ";
    res += bank0_[i].size() ? bank0_[i] : "-";
    if (i != 7)
      res += ", ";
  }

  return res;
}

size_t RegsBank0::GetIndexOfVar(string name) {
  for (size_t i = 0; i < bank0_.size(); i++)
    if (bank0_[i] == name)
      return i;
  return npos;
}
