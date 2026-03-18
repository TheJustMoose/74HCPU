#pragma once

#include <string>

struct Operation {
  std::string res_arg {};
  bool res_in_temp {false};
  std::string op_name {};
  std::string left_arg {};
  bool left_arg_is_num {false};  // left - because table containt: res|left|op|right
  std::string right_arg {};      // so when we have x = a + b, a is left
  bool removed {false};          // but when x = a, a is left again!

  Operation(std::string res, std::string op, std::string l, bool l_is_num, std::string r, bool in_temp = false)
    : res_arg(res), op_name(op), left_arg(l), left_arg_is_num(l_is_num), right_arg(r), res_in_temp(in_temp) {}

  std::string str() {
    std::string suffix {res_in_temp ? " (res_in_temp)" : ""};
    if (removed)
      suffix += " - removed";
    if (left_arg.size())
      return (res_arg.size() ? res_arg : "?") + " = "
             + left_arg + " " + op_name + " " + right_arg + suffix;
    else
      return (res_arg.size() ? res_arg : "?") + " = "
             + op_name + " " + right_arg + suffix;
  }

  std::string align(std::string s) {
    std::string res{s};
    while (res.size() < 4)
      res = " " + res;
    return res;
  }

  std::string raw() {
    return "|" + align(res_arg) + " |" + align(left_arg) + " |" +
           (left_arg_is_num ? " num" : "    ") + " | " +
           align(op_name) + "|" + align(right_arg) + " | " +
           (res_in_temp ? "tmp" : "var") + " |";
  }

  int argNum() {
    return !left_arg.empty() + !right_arg.empty();
  }

  void clear() {
    removed = true;
  }
};
