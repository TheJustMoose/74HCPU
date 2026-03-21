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
  bool left_arg_is_num {false};
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

vector<string> Assemble(vector<Operation> code) {
  vector<string> res;

  for (Operation op : code) {
    cout << op.raw() << " " << op.str() << endl;
    if (!op.op_name.size()) {
      // bool getVar(std::string var_name, Var& var);
      Var v;
      if (!getVar(op.res_arg, v)) {
        cout << "Variable " << op.res_arg << " was not declared" << endl;
        continue;
      }

      if (v.is_ptr) {
        string res_reg = FindPtrFor(op.res_arg);
        string line1 = op.res_arg + " = " + op.left_arg;
        res.push_back(line1);

        string lval;
        string hval;
        if (op.left_arg_is_num) {
          int val = std::stoi(op.left_arg);
          lval = ToHexString(val & 0xFF, 3) + "h";
          hval = ToHexString((val >> 8) & 0xFF, 3) + "h";
        } else {
          string left = FindPtrFor(op.left_arg);
          lval = left + "L";
          hval = left + "H";
        }

        string line11 = "mov " + res_reg + "L, " + lval;
        res.push_back(line11);
        string line12 = "mov " + res_reg + "H, " + hval;
        res.push_back(line12);
      } else {
        string res_reg = FindRegFor(op.res_arg);
        string line1 = op.res_arg + " = " + op.left_arg;
        res.push_back(line1);
  
        string line11 = "mov " + res_reg + ", " +
                        (op.left_arg_is_num ? op.left_arg : FindRegFor(op.left_arg)) +
                        "   " + DumpRegs();
        res.push_back(line11);
      }
    } else if (!op.left_arg.size()) {  // t1 = -t1
      string line1 = op.right_arg + " = " + op.op_name + op.right_arg;
      res.push_back(line1);            // b = t1
      string line2 = op.res_arg + " = " + op.right_arg;
      res.push_back(line2);

      res.push_back("asm will be here soon (op.left_arg is empty)");
    } else {
      string line1 = op.res_arg + " = " + op.left_arg;
      res.push_back(line1);
      string line2 = op.res_arg + " " + op.op_name + "= " + op.right_arg;
      res.push_back(line2);

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
