#include "backend.h"
#include "parser.h"
#include "regs_bank.h"
#include "spillable.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

/*
mul R7, R5            // t4 *= f   // R0: a, R1: b, R2: c, R3: e, R4: 5, R5: f, R6: g, R7: t4
out  CPU_FLAGS, 0     // switch to bank 0
mov XL, 0x0010
mov XH, 0x0000
out  CPU_FLAGS, 0x40  // switch to bank 1
st X, R6              // g
mov R6, R0            // t5 = a    // R0: a, R1: b, R2: c, R3: e, R4: 5, R5: f, R6: t5, R7: t4
add R6, R7            // t5 += t4  // R0: a, R1: b, R2: c, R3: e, R4: 5, R5: f, R6: t5, R7: t4
mov R2, R6            // c = t5    // R0: a, R1: b, R2: c, R3: e, R4: 5, R5: f, R6: t5, R7: -

// а не надо ли где-то здесь читать переменную g обратно из памяти ???
add R2, R7            // c += g    // R0: a, R1: b, R2: c, R3: e, R4: 5, R5: f, R6: t5, R7: g

mov R6, R0            // t7 = a    // R0: a, R1: b, R2: c, R3: e, R4: 5, R5: f, R6: t7, R7: g
*/

// copied from asm
string ToUpper(string s) {
  transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

string AlignNumber(string s, int width) {
  string res {s};
  while (res.size() < width)
    res = "0" + res;
  return res;
}

string AlignByWidth(string s, int width, char c = ' ') {
  string res {s};
  string fll({c});
  while (res.size() < width)
    res += fll;
  return res;
}

string ToHexString(int value, int width = 4) {
  stringstream ss;
  ss << hex << value;
  return AlignNumber(ToUpper(ss.str()), width);
}

class RegSpillable: public ISpillable {
 public:
  RegSpillable(Backend* backend)
    : backend_(backend) {}

  void Spill(size_t reg_idx, uint16_t var_addr,
             vector<string> &res, string var_name) override;

 private:
  Backend* backend_ {nullptr};
};

void RegSpillable::Spill(size_t reg_idx, uint16_t var_addr,
                         vector<string> &res, string var_name) {
  cout << "// No free registers. Will spill R" << reg_idx
       << " with " << var_name
       << " to addr " << var_addr << endl;

  if (!backend_) {
    cout << "backend_ is NULL" << endl;
    return;
  }

  backend_->SwitchToBank1();
  backend_->AddAsmInstruction(string("mov XL, 0x") + ToHexString(var_addr & 0xFF));
  backend_->AddAsmInstruction(string("mov XH, 0x") + ToHexString((var_addr >> 8) & 0xFF));
  backend_->SwitchToBank0();
  backend_->AddAsmInstruction(string("st X, R") + to_string(reg_idx), var_name);
}

size_t reg_cnt = 8;
vector<string> bank1(reg_cnt);

string FindPtrFor(string var_name) {
  // check existing pair ptr:reg
  if (bank1[0] == var_name)
    return "X";
  if (bank1[2] == var_name)
    return "Y";

  // try to insert new pair ptr:reg
  if (!bank1[0].size()) {
    bank1[0] = var_name;
    return "X";
  } else if (!bank1[2].size()) {
    bank1[2] = var_name;
    return "Y";
  } else {
  }

  // oops
  return "";
}

void Backend::AddAsmInstruction(string instr, string cmnt, string cmnt2) {
  string res {instr};
  if (cmnt.size()) {
    res = AlignByWidth(res, 22);
    res += "// ";
    res += AlignByWidth(cmnt, 10);
    res += cmnt2;
  }
  res_asm_.push_back(res);
}

void Backend::SwitchToBank0() {
  AddAsmInstruction("out  CPU_FLAGS, 0x40", "switch to bank 1");
}

void Backend::SwitchToBank1() {
  AddAsmInstruction("out  CPU_FLAGS, 0", "switch to bank 0");
}

void Backend::FreeTheRegisters(RegsBank0& bank0, const Operation& op) {
  size_t var_cnt = op.live_out_vars.size();  // this variables are still alive after instruction
  for (size_t i = 0; i < var_cnt; i++) {
    if (op.live_in_vars[i] && !op.live_out_vars[i]) {  // var has died
      string reg_name = idx_to_var_[i];
      size_t idx = bank0.GetIndexOfVar(reg_name);
      cout << "variable with idx == " << i
           << ", var_name: " << bank0[idx] << " has died" << endl;
      cout << "reg_name: " << reg_name << ", reg_idx: " << idx << endl;
      bank0.FreeTheRegister(idx);
      cout << bank0.DumpRegs() << endl;
    }
  }
}

void Backend::GenerateAssignment(RegsBank0& bank0, Operation op, Var& v) {
  if (v.is_ptr) {  // pointer ops
    string lval;
    string hval;
    if (op.arg_is_num) {  // pointer1 = 0x1000
      int val = std::stoi(op.left_arg);
      lval = ToHexString(val & 0xFF, 3) + "h";
      hval = ToHexString((val >> 8) & 0xFF, 3) + "h";
    } else {  // pointer1 = pointer2
      string left = FindPtrFor(op.left_arg);
      lval = left + "L";
      hval = left + "H";
    }

    string cmnt1 = op.res_arg + " = " + op.left_arg;

    string res_reg = FindPtrFor(op.res_arg);
    SwitchToBank1();
    string line11 = "mov " + res_reg + "L, " + lval;
    AddAsmInstruction(line11, cmnt1);
    string line12 = "mov " + res_reg + "H, " + hval;
    AddAsmInstruction(line12);
    SwitchToBank0();
  } else {  // ordinary regs
    string cmnt1 = op.res_arg + " = " + op.left_arg;

    string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
    string line11 = "mov " + res_reg + ", " +
                    (op.arg_is_num ? op.left_arg : bank0.FindRegFor(op.left_arg, res_asm_));
    AddAsmInstruction(line11, cmnt1, bank0.DumpRegs());
  }
}

void Backend::GenerateInvertion(RegsBank0& bank0, Operation op) {
  // TODO: check it!
  if (op.arg_is_num &&
      op.right_arg.size() &&
      op.op_name == "-")
    op.right_arg = "-" + op.right_arg;

  string cmnt1 = op.res_arg + " = " + op.right_arg;

  string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
  if (op.arg_is_num) {  // pointer1 = 0x1000
    string line = "mov " + res_reg + ", " + op.right_arg;
    AddAsmInstruction(line, cmnt1);
  } else {  // R0 = -R1
    // mov R0, ~R1; add R0, 1        // v1
    // xor R0, R0; addc R0, ~R1 + 1  // v2
    string line21 = "mov " + res_reg + ", ~" + bank0.FindRegFor(op.right_arg, res_asm_);
    AddAsmInstruction(line21, cmnt1);
    string line22 = "add " + res_reg + ", 1";
    AddAsmInstruction(line22);
  }
}

void Backend::GenerateArithmOps(RegsBank0& bank0, Operation op) {
  // c = a + b   -->   c = a, c += b
  // what about var_size ?
  string cmnt1 = op.res_arg + " = " + op.left_arg;  // c = a

  string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
  string line11 = "mov " + res_reg + ", " + bank0.FindRegFor(op.left_arg, res_asm_);
  AddAsmInstruction(line11, cmnt1, bank0.DumpRegs());

  string cmnt2 = op.res_arg + " " + op.op_name + "= " + op.right_arg;  // c += b

  string cmd {"unk"};
  if (op.op_name == "+")
    cmd = "add";
  else if (op.op_name == "-")
    cmd = "sub";
  else if (op.op_name == "*")
    cmd = "mul";
  else if (op.op_name == "/")
    cmd = "/ - not implemented!";

  string line21 = cmd + " " + res_reg + ", " + bank0.FindRegFor(op.right_arg, res_asm_);
  AddAsmInstruction(line21, cmnt2, bank0.DumpRegs());
}

void Backend::GenerateCode(vector<Operation> code) {
  RegSpillable reg_spillable(this);
  RegsBank0 bank0(&reg_spillable, var_addrs_);

  res_asm_.clear();

  for (Operation op : code) {
    if (!op.op_name.size()) {  // empty op_name means assignment
      Var v;
      if (!getVar(op.res_arg, v)) {
        cout << "Variable " << op.res_arg << " was not declared" << endl;
        continue;
      }
      GenerateAssignment(bank0, op, v);
    } else if (!op.left_arg.size()) {  // assign ops (t1 = -t1)
      GenerateInvertion(bank0, op);
    } else {  // arithm ops (left_arg is not empty)
      GenerateArithmOps(bank0, op);
    }

    FreeTheRegisters(bank0, op);
  }
}
