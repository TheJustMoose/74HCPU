#pragma once

#include <string>

struct Operation {
  std::string res_arg {};
  std::string name {};
  std::string left_arg {};
  std::string right_arg {};

  Operation(std::string res, std::string n, std::string l, std::string r)
    : res_arg(res), name(n), left_arg(l), right_arg(r) {}

  std::string str() {
    return (res_arg.size() ? res_arg : "?") + " = "
           + left_arg + " " + name + " " + right_arg;
  }
};
