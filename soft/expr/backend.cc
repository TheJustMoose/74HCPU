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

// copied from asm
string ToUpper(string s) {
  transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

string ToHexString(int value, int width = 4) {
  stringstream ss;
  ss << hex << value;
  string res = ToUpper(ss.str());
  while (res.size() < width)
    res = "0" + res;
  return res;
}

class RegSpillable: public ISpillable {
 public:
  RegSpillable(Backend* backend)
    : backend_(backend) {}

  void Spill(size_t reg_idx, uint16_t var_addr,
             vector<string> &res) override;

 private:
  Backend* backend_ {nullptr};
};

void RegSpillable::Spill(size_t reg_idx, uint16_t var_addr,
                         vector<string> &res) {
  cout << "// No free registers. Will spill R" << reg_idx
       << " to addr " << var_addr << endl;

  if (backend_) {
    backend_->SwitchToBank1();
    backend_->AddAsmInstruction(string("mov XL, 0x") + ToHexString(var_addr & 0xFF));
    backend_->AddAsmInstruction(string("mov XH, 0x") + ToHexString((var_addr >> 8) & 0xFF));
    backend_->SwitchToBank0();
    backend_->AddAsmInstruction(string("st X, R") + to_string(reg_idx));
  }

/*
TODO: конвертнуть вот это в макрос:
out  CPU_FLAGS, 0
mov XL, 0x0002
mov XH, 0x0000
out  CPU_FLAGS, 0x40
*/
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

void Backend::AddAsmInstruction(string instr, string cmnt) {
  res_asm_.push_back(instr);
}

void Backend::AddComment(string cmnt) {
  res_asm_.push_back(cmnt);
}

void Backend::SwitchToBank0() {
  AddAsmInstruction("out  CPU_FLAGS, 0x40");
}

void Backend::SwitchToBank1() {
  AddAsmInstruction("out  CPU_FLAGS, 0");
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

    string res_reg = FindPtrFor(op.res_arg);
    SwitchToBank1();
    string line11 = "mov " + res_reg + "L, " + lval;
    AddAsmInstruction(line11);
    string line12 = "mov " + res_reg + "H, " + hval;
    AddAsmInstruction(line12);
    SwitchToBank0();

    string cmnt1 = op.res_arg + " = " + op.left_arg;
    AddComment(cmnt1);
  } else {  // ordinary regs
    string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
    string line11 = "mov " + res_reg + ", " +
                    (op.arg_is_num ? op.left_arg : bank0.FindRegFor(op.left_arg, res_asm_)) +
                    "   " + bank0.DumpRegs();
    AddAsmInstruction(line11);

    string cmnt1 = op.res_arg + " = " + op.left_arg;
    AddComment(cmnt1);
  }
}

void Backend::GenerateInvertion(RegsBank0& bank0, Operation op) {
  // TODO: check it!
  if (op.arg_is_num &&
      op.right_arg.size() &&
      op.op_name == "-")
    op.right_arg = "-" + op.right_arg;

  string res_reg = bank0.FindRegFor(op.res_arg, res_asm_);
  if (op.arg_is_num) {  // pointer1 = 0x1000
    string line = "mov " + res_reg + ", " + op.right_arg;
    AddAsmInstruction(line);
  } else {  // R0 = -R1
    // mov R0, ~R1; add R0, 1        // v1
    // xor R0, R0; addc R0, ~R1 + 1  // v2
    string line21 = "mov " + res_reg + ", ~" + bank0.FindRegFor(op.right_arg, res_asm_);
    AddAsmInstruction(line21);
    string line22 = "add " + res_reg + ", 1";
    AddAsmInstruction(line22);
  }

  string cmnt1 = op.res_arg + " = " + op.right_arg;
  AddComment(cmnt1);
}

void Backend::GenerateArithmOps(RegsBank0& bank0, Operation op) {
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
  AddAsmInstruction(line21);

  // c = a + b   -->   c = a, c += b
  string cmnt1 = op.res_arg + " = " + op.left_arg;  // c = a
  AddComment(cmnt1);
  string cmnt2 = op.res_arg + " " + op.op_name + "= " + op.right_arg;  // c += b
  AddComment(cmnt2);
}

void Backend::GenerateCode(vector<Operation> code) {
  RegSpillable reg_spillable(this);
  RegsBank0 bank0(&reg_spillable, var_addrs_);

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
