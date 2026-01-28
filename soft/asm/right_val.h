#pragma once

#include <cstdint>
#include <string>

#include "error_collector.h"
#include "reg.h"
#include "str_util.h"

class RightVal {
 public:
  RightVal() = default;

  void Init(int line_number, std::string right) {
    line_number_ = line_number;
    right_ = right;
    Parse();
  }

  void Parse() {
    if (right_.find("+1") != std::string::npos) {
      force_cf_ = true;
      right_.resize(right_.size() - 2);
    }

    // It's duplicate of StripPrefix but for immediate values:
    if (right_.find("LO(") == 0) {
      if (*right_.rbegin() != ')')
        ErrorCollector::GetInstance().err("Can't find right parentheses - )",
                                          line_number_);
      right_ = right_.substr(3, right_.size() - 4);
      lo_macro_ = true;
    } else if (right_.find("HI(") == 0) {
      if (*right_.rbegin() != ')')
        ErrorCollector::GetInstance().err("Can't find right parentheses - )",
                                          line_number_);
      right_ = right_.substr(3, right_.size() - 4);
      hi_macro_ = true;
    }

    int t = 0;
    immediate_ = StrToInt(right_, &t);  // MOV R1, 10
    if (immediate_) {
      if (t < 0)
        right_val_ = t & 0x00FF;  // TODO: how LO() should work when value is negative?
      else {
        if (lo_macro_)
          right_val_ = t & 0xFF;
        else if (hi_macro_)
          right_val_ = (t >> 8) & 0xFF;
        else
          right_val_ = t;
      }

      if (right_val_> 0xFF)
        ErrorCollector::GetInstance().err(
          "Error. Immediate value should have 8 bit only (0 - 255). Got: " + right_, line_number_);
    } else {  // not val, try to check register name
      std::string rest = StripPrefix(right_);
      right_reg_ = Names::RegFromName(rest);  // MOV R0, R1
      if (right_reg_ == rUnkReg) {
        ErrorCollector::GetInstance().err(
          "You have to use Register name or immediate value as rval. Now it's: " + rest, line_number_);
      }
    }
  }

  std::string StripPrefix(std::string val) {
    if (val.size() <= 2)  // strlen("R0") == 2
      return val;

    if (val[0] == '`' || val[1] == '`')
      inv_hi_ = true;
    if (val[0] == '\'' || val[1] == '\'')
      inv_lo_ = true;

    bool inv_all {false};
    if (val[0] == '~' || val[1] == '~')
      inv_all = true;

    int cnt = inv_lo_ + inv_hi_ + inv_all;
    if (cnt > 2)
      cnt = 2;
    if (cnt)
      val = val.substr(cnt, val.size() - cnt);

    if (inv_all) {
      inv_hi_ = true;
      inv_lo_ = true;
    }

    if (val.find("L(") == 0 && *val.rbegin() == ')') {
      zero_hi_ = true;
      return val.substr(2, val.size() - 3);
    }
    if (val.find("H(") == 0 && *val.rbegin() == ')') {
      zero_lo_ = true;
      return val.substr(2, val.size() - 3);
    }
    return val;
  }

  bool immediate() { return immediate_; }
  uint16_t val() { return right_val_; }
  REG reg() { return right_reg_; }
  int LineNumber() { return line_number_; }
  std::string str_val() { return right_; }

  bool zero_lo() { return zero_lo_; }
  bool zero_hi() { return zero_hi_; }
  bool inv_lo() { return inv_lo_; }
  bool inv_hi() { return inv_hi_; }
  bool force_cf() { return force_cf_; }
  bool lo_macro() { return lo_macro_; }
  bool hi_macro() { return hi_macro_; }

  void update_val(uint16_t val) {
    right_val_ = val;
    immediate_ = true;
  }

 private:
  bool immediate_ {false};
  uint16_t right_val_ {0};
  REG right_reg_ {rUnkReg};
  int line_number_ {0};
  bool zero_lo_ {false};
  bool zero_hi_ {false};
  bool inv_lo_ {false};
  bool inv_hi_ {false};
  bool force_cf_ {false};
  bool lo_macro_ {false};
  bool hi_macro_ {false};
  std::string right_;
};
