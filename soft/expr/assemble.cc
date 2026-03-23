#include "assemble.h"
#include "parser.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

/*
  std::string res_arg {};
  bool res_in_temp {false};
  std::string op_name {};
  std::string left_arg {};
  bool arg_is_num {false};
  std::string right_arg {};
  bool removed {false};
*/

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

size_t reg_cnt = 8;
vector<string> bank0(reg_cnt);
vector<string> bank1(reg_cnt);

string FindRegFor(string var_name) {
  // check existing pair var:reg
  for (size_t i = 0; i < 8; i++)
    if (bank0[i] == var_name)
      return "R" + to_string(i);

  // try to insert new pair var:reg
  for (size_t i = 0; i < 8; i++)
    if (!bank0[i].size()) {
      bank0[i] = var_name;
      return "R" + to_string(i);
    }

  // oops
  return "";
}

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
  }
  if (!bank1[2].size()) {
    bank1[2] = var_name;
    return "Y";
  }

  // oops
  return "";
}

string DumpRegs() {
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

void SwitchToBank0(vector<string> &res) {
  res.push_back("out  CPU_FLAGS, 0x40");
}

void SwitchToBank1(vector<string> &res) {
  res.push_back("out  CPU_FLAGS, 0");
}

vector<string> Assemble(vector<Operation> code) {
  vector<string> res;

  for (Operation op : code) {
    cout << op.raw() << " " << op.str() << endl;
    if (!op.op_name.size()) {  // empty op_name means assignment
      Var v;
      if (!getVar(op.res_arg, v)) {
        cout << "Variable " << op.res_arg << " was not declared" << endl;
        continue;
      }

      if (v.is_ptr) {  // pointer ops
        string line1 = op.res_arg + " = " + op.left_arg;
        res.push_back(line1);

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
        SwitchToBank1(res);
        string line11 = "mov " + res_reg + "L, " + lval;
        res.push_back(line11);
        string line12 = "mov " + res_reg + "H, " + hval;
        res.push_back(line12);
        SwitchToBank0(res);
      } else {
        string line1 = op.res_arg + " = " + op.left_arg;
        res.push_back(line1);
  
        string res_reg = FindRegFor(op.res_arg);
        string line11 = "mov " + res_reg + ", " +
                        (op.arg_is_num ? op.left_arg : FindRegFor(op.left_arg)) +
                        "   " + DumpRegs();
        res.push_back(line11);
      }
    } else if (!op.left_arg.size()) {  // assign ops (t1 = -t1)
      res.push_back(op.raw());
      string line1 = op.res_arg + " = " + op.op_name + op.right_arg + " <<<<<";
      res.push_back(line1);

      string res_reg = FindRegFor(op.res_arg);  // как бы сделать так, чтобы следующий вызов FindRegFor не сломал res_reg?
      if (op.arg_is_num) {  // pointer1 = 0x1000
        res.push_back("Okay, We are in the else branch");
        /*int val = std::stoi(op.right_arg);
        val = ~val;
        val++;  // two's complement negative value!
        string neg_val = to_string(val);*/
        string line = "mov " + res_reg + ", " + op.right_arg;
        res.push_back(line);
      } else {  // R0 = -R1
        // mov R0, ~R1; add R0, 1
        // xor R0, R0; addc R0, ~R1 + 1
        string line21 = "mov " + res_reg + ", ~" + FindRegFor(op.right_arg);
        res.push_back(line21);
        string line22 = "add " + res_reg + ", 1";
        res.push_back(line22);
      }
      // mov R0, ival; add R0, 1
      // просто пересчитать в компиляторе отрицательное значение в дополнительный код)
    } else {  // arithm ops (left_arg is not empty)
      // c = a + b   -->   c = a, c += b
      string line1 = op.res_arg + " = " + op.left_arg;  // c = a
      res.push_back(line1);
      string line2 = op.res_arg + " " + op.op_name + "= " + op.right_arg;  // c += b
      res.push_back(line2);

      // what about var_size ?
      string res_reg = FindRegFor(op.res_arg);
      string line11 = "mov " + res_reg + ", " + FindRegFor(op.left_arg) + "   " + DumpRegs();
      res.push_back(line11);

      string cmd {"unk"};
      if (op.op_name == "+")
        cmd = "add";
      else if (op.op_name == "-")
        cmd = "sub";
      else if (op.op_name == "*")
        cmd = "mul";
      else if (op.op_name == "/")
        cmd = "/ - not implemented!";

      string line21 = cmd + " " + res_reg + ", " + FindRegFor(op.right_arg) + "   " + DumpRegs();
      res.push_back(line21);
    }
  }

  return res;
}
