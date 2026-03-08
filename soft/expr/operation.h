#pragma once

#include <string>

struct Operation {
  std::string res_arg {};
  std::string op_name {};
  std::string left_arg {};
  std::string right_arg {};
  bool res_in_temp {false};

  Operation(std::string res, std::string op, std::string l, std::string r, bool in_temp = false)
    : res_arg(res), op_name(op), left_arg(l), right_arg(r), res_in_temp(in_temp) {}

  std::string str() {
    std::string suffix {res_in_temp ? " res_in_temp" : ""};
    if (left_arg.size())
      return (res_arg.size() ? res_arg : "?") + " = "
             + left_arg + " " + op_name + " " + right_arg + suffix;
    else
      return (res_arg.size() ? res_arg : "?") + " = "
             + op_name + " " + right_arg + suffix;
  }

  int argNum() {
    return !left_arg.empty() + !right_arg.empty();
  }
};
