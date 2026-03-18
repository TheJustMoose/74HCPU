#include "assemble.h"

#include <iostream>

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

string FindFreeRegFor(string var_name) {
  for (size_t i = 0; i < 8; i++)
    if (!bank0[i].size()) {
      bank0[i] = var_name;
      return "R" + to_string(i);
    }

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

vector<string> Assemble(vector<Operation> code) {
  vector<string> res;

  for (Operation op : code) {
    if (!op.op_name.size()) {
      cout << "Hooray!!!" << endl;
      string left_reg = FindFreeRegFor(op.left_arg);  // this is immediate value :(((
      string res_reg = FindFreeRegFor(op.res_arg);

      // надо как-то протащить занение о том, что это число, а не регистр,
      // от лексера до кодогенератора Оо
      string line1 = op.res_arg + " = " + op.left_arg;
      res.push_back(line1);

      // мне нужно как-то запомнить, что -res_reg- right_arg содержит число, а не переменную
      // иначе не понятно, нужно ли вызывать FindFreeRegFor, или нет!
      // то есть, op.right_arg должна хранить не только имя, но и bool число/не число
      string line11 = "mov " + res_reg + ", " +
                      (op.left_arg_is_num ? op.left_arg : left_reg) +
                      "   " + DumpRegs();
      res.push_back(line11);
    } else if (!op.left_arg.size()) {  // t1 = -t1
      string line1 = op.right_arg + " = " + op.op_name + op.right_arg;
      res.push_back(line1);            // b = t1
      string line2 = op.res_arg + " = " + op.right_arg;
      res.push_back(line2);
    } else {
      string line1 = op.res_arg + " = " + op.left_arg;
      res.push_back(line1);
      string line2 = op.res_arg + " " + op.op_name + "= " + op.right_arg;
      res.push_back(line2);
    }
  }

  return res;
}
