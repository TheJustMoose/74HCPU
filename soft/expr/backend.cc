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
  b = -t1
    ?
  b = 0
  b -= t1
    vs
  b = t1
  b = -b
    vs
  t1 = -t1
  b = t1
*/

class RegSpillable: public ISpillable {
 public:
  RegSpillable(map<string, uint16_t>& var_addrs)
    : var_addrs_(var_addrs) {}

  void Spill(size_t reg_idx, /*string some_var_name,*/ vector<string> &res) override;

 private:
  map<string, uint16_t>& var_addrs_;
};

void RegSpillable::Spill(size_t reg_idx, /*string some_var_name,*/ vector<string> &res) {
  // add instruction to spilling reg
  // ptr V = find_addr_of(some_var_name?)

  //string line = "ST V, " + reg_idx;
  //res.push_back(line);
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

// copied from asm
string ToUpper(string s) {
  transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

string ToHexString(int value, int width) {
  stringstream ss;
  ss << hex << value;
  string res = ToUpper(ss.str());
  while (res.size() < width)
    res = "0" + res;
  return res;
}

/*
out  CPU_FLAGS, 0x40  ; BF = 1
mov  SPL, 0xFF    ; init stack
mov  SPH, 0xFF
mov  VL, 0        ; init Video RAM ptr
mov  VH, 0
out  CPU_FLAGS, 0 ; BF = 0
*/

void Backend::AddAsmInstruction(string instr) {
  res_asm_.push_back(instr);
}

void Backend::SwitchToBank0() {
  AddAsmInstruction("out  CPU_FLAGS, 0x40");
}

void Backend::SwitchToBank1() {
  AddAsmInstruction("out  CPU_FLAGS, 0");
}

void Backend::GenerateAssignment(RegsBank0& bank0, Operation op, Var& v) {
  if (v.is_ptr) {  // pointer ops
    string line1 = op.res_arg + " = " + op.left_arg;
    res_asm_.push_back(line1);

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

    string res_reg = FindPtrFor(op.res_arg);
    SwitchToBank1();
    string line11 = "mov " + res_reg + "L, " + lval;
    res_asm_.push_back(line11);
    string line12 = "mov " + res_reg + "H, " + hval;
    res_asm_.push_back(line12);
    SwitchToBank0();
  } else {  // ordinary regs
    string line1 = op.res_arg + " = " + op.left_arg;
    res_asm_.push_back(line1);

    string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
    string line11 = "mov " + res_reg + ", " +
                    (op.arg_is_num ? op.left_arg : bank0.FindRegFor(op.left_arg, res_asm_)) +
                    "   " + bank0.DumpRegs();
    AddAsmInstruction(line11);
  }
}

void Backend::GenerateInvertion(RegsBank0& bank0, Operation op) {
  res_asm_.push_back(op.raw());
  string line1 = op.res_arg + " = " + op.op_name + op.right_arg + " <<<<<";
  res_asm_.push_back(line1);

  string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
  if (op.arg_is_num) {  // pointer1 = 0x1000
    string line = "mov " + res_reg + ", " + op.right_arg;
    AddAsmInstruction(line);
  } else {  // R0 = -R1
    // mov R0, ~R1; add R0, 1
    // xor R0, R0; addc R0, ~R1 + 1
    string line21 = "mov " + res_reg + ", ~" + bank0.FindRegFor(op.right_arg, res_asm_);
    AddAsmInstruction(line21);
    string line22 = "add " + res_reg + ", 1";
    AddAsmInstruction(line22);
  }
  // mov R0, ival; add R0, 1
  // просто пересчитать в компиляторе отрицательное значение в дополнительный код)
}

void Backend::GenerateArithmOps(RegsBank0& bank0, Operation op) {
  // c = a + b   -->   c = a, c += b
  string line1 = op.res_arg + " = " + op.left_arg;  // c = a
  res_asm_.push_back(line1);
  string line2 = op.res_arg + " " + op.op_name + "= " + op.right_arg;  // c += b
  res_asm_.push_back(line2);

  // what about var_size ?
  string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
  string line11 = "mov " + res_reg + ", " + bank0.FindRegFor(op.left_arg, res_asm_)
                + "   " + bank0.DumpRegs();
  AddAsmInstruction(line11);

  string cmd {"unk"};
  if (op.op_name == "+")
    cmd = "add";
  else if (op.op_name == "-")
    cmd = "sub";
  else if (op.op_name == "*")
    cmd = "mul";
  else if (op.op_name == "/")
    cmd = "/ - not implemented!";

  string line21 = cmd + " " + res_reg + ", " + bank0.FindRegFor(op.right_arg, res_asm_)
                + "   " + bank0.DumpRegs();
  res_asm_.push_back(line21);
}

void Backend::GenerateCode(vector<Operation> code) {
  RegSpillable reg_spillable(var_addrs_);
  RegsBank0 bank0(&reg_spillable);

  res_asm_.clear();

  for (Operation op : code) {
    cout << op.raw() << " " << op.str() << endl;
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
  }
}
