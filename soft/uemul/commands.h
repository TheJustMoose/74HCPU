#pragma once

#include <stdint.h>
#include <string>

#include "names.h"

class Cmd {
 public:
  Cmd(uint16_t cmd): cmd_(cmd) {}

  uint16_t cmd() { return cmd_; }
  uint8_t op() { return (cmd_ >> 8) & 0xF0; }

  virtual std::string Params() { return ""; }

 protected:
  uint16_t cmd_ {0};
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|   ADD |  DST |C| SRC |-|Z|z|I|i|
class ArithmCmd: public Cmd {
 public:
  ArithmCmd(uint16_t cmd): Cmd(cmd) {}

  uint8_t dst() { return (cmd_ >> 9) & 0x07; }
  bool is_cnst() { return (cmd_ >> 8) & 0x01; }
  uint8_t src() { return (cmd_ >> 5) & 0x07; }
  uint8_t cnst() { return (cmd_ & 0xFF); }

  virtual std::string Params() {
    std::string rsrc = is_cnst() ? std::to_string(cmd_ & 0xFF) : RegNames[src()];
    return std::string(" ") + RegNames[dst()] + std::string(", ") + rsrc;
  }
};
