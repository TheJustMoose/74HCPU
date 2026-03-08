#pragma once

#include <string>

struct Operation {
  std::string name {};
  std::string left_arg {};
  std::string right_arg {};

  Operation(std::string n, std::string l, std::string r)
    : name(n), left_arg(l), right_arg(r) {}

  std::string str() {
    return left_arg + " " + name + " " + right_arg;
  }
};
