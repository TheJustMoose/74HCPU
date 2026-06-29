#pragma once

#include <map>
#include <string>
#include <vector>

// Indicates which column of the Operation structure is filled in.
enum NumPos {npNone, npLeft, npRight, npBoth};

inline std::string NumPos2String(NumPos np) {
  switch (np) {
    case npNone:  return " none";
    case npLeft:  return " left";
    case npRight: return "right";
    case npBoth:  return " both";
    default: return "NumPos error!";
  }
}

enum OperationType {
  otBinaryArithm,
  otUnaryArithm,
  otAssignment,
  otRelational
};

inline std::string OpType2String(OperationType ot) {
  switch (ot) {
    case otBinaryArithm: return "binop";
    case otUnaryArithm: return " unop";
    case otAssignment: return "assig";
    case otRelational: return "relat";
    default: return "unknown OperationType";
  }
}

class Operation {
 private:
  NumPos num_pos_ = npNone;
  OperationType op_type_ = otBinaryArithm;

 public:
  std::string res_arg {};
  bool res_in_temp {false};
  std::string op_name {};
  std::string left_arg {};   // what is left?
                             // the table contains: res|left|op|right
  std::string right_arg {};  // so when we have x = a + b, a is left
  bool removed {false};      // but when x = a, a is left again!

  std::vector<bool> live_in_vars;   // live vars before Operation executing (LIVE_in)
  std::vector<bool> live_out_vars;  // live vars after Operation executing (LIVE_out)

  Operation(std::string res, std::string op, std::string l, NumPos num_pos,
            std::string r, OperationType op_type, bool in_temp = false)
    : res_arg(res), op_name(op), left_arg(l), num_pos_(num_pos), op_type_(op_type),
      right_arg(r), res_in_temp(in_temp) {}

  std::string str() const {
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

  std::string align(std::string s, int maxw = 4) const {
    std::string res{s};
    while (res.size() < maxw)
      res = " " + res;
    return res;
  }

  std::string raw() const {
    return "|" + align(res_arg) + " | = |" + align(left_arg) + " |" +
           align(op_name) + " |" + align(right_arg) + " | " +
           align(NumPos2String(num_pos_)) + " | " +
           align(OpType2String(op_type_)) + " |";
  }

  std::string live_vars_str() const {
    std::string res;
    res.resize(live_out_vars.size());
    for (size_t i = 0; i < live_out_vars.size(); i++)
      res[i] = live_out_vars[i] ? '1' : '0';
    return res;
  }

  std::string live_vars_str(std::map<size_t, std::string> idx_to_var) const {
    std::string res("{");

    for (size_t i = 0; i < live_out_vars.size(); i++)
      if (live_out_vars[i]) {
        res += idx_to_var[i];
        res += ",";
      }

    if (*res.rbegin() == ',')  // res has at least 1 char
      *res.rbegin() = '}';
    else
      res += "}";

    return res;
  }

  int argNum() const {
    return !left_arg.empty() + !right_arg.empty();
  }

  NumPos num_pos() {
    return num_pos_;
  }

  OperationType op_type() {
    return op_type_;
  }

  void clear() {
    removed = true;
  }
};
