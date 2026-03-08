#pragma once

#include <string>

struct Operation {
  std::string res_arg {};
  std::string op_name {};
  std::string left_arg {};
  std::string right_arg {};

  Operation(std::string res, std::string op, std::string l, std::string r)
    : res_arg(res), op_name(op), left_arg(l), right_arg(r) {}

  std::string str() {
    if (left_arg.size())
      return (res_arg.size() ? res_arg : "?") + " = "
             + left_arg + " " + op_name + " " + right_arg;
    else
      return (res_arg.size() ? res_arg : "?") + " = "
             + op_name + " " + right_arg;
  }

  int argNum() {
    return !left_arg.empty() + !right_arg.empty();
  }
};
