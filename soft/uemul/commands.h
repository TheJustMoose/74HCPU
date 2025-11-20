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

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|   UNO |  DST |0|-|TYP|F|-|-|-|-| 60 0110 0000|*| унарные команды не используют операнд SRC, поэтому нельзя использовать инверторы и отдельные нибблы
class UnaryCmd: public Cmd {
 public:
  UnaryCmd(uint16_t cmd): Cmd(cmd) {}

  uint8_t dst() { return (cmd_ >> 9) & 0x07; }
  uint8_t type() { return (cmd_ >> 5) & 0x03; }

  virtual std::string Params() {
    std::string res = UnoNames[type()];
    res += " ";
    res += RegNames[dst()];
    return res;
  }
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|    LD |  DST |0|EXT|D|U|OFFSET4| 90 1001 0000| |
//|    ST |  SRC |0|EXT|D|U|OFFSET4| C0 1100 0000| |
class MemoryCmd: public Cmd {
 public:
  MemoryCmd(uint16_t cmd): Cmd(cmd) {}

  uint8_t reg() { return (cmd_ >> 9) & 0x07; }
  uint8_t ptr() { return (cmd_ >> 6) & 0x03; }
  uint8_t autodec() { return (cmd_ >> 5) & 0x01; }
  uint8_t autoinc() { return (cmd_ >> 4) & 0x01; }
  uint8_t offs() { return cmd_ & 0x0F; }

  virtual std::string Params() {
    std::string suffix;
    if (autodec())
      suffix += "D";
    if (autoinc())
      suffix += "I";
    if (offs()) {
      suffix += " + ";
      suffix += std::to_string(offs());  // offs may have value from -8 to +7 so I have to convert it to "int"
    }

    if (op() == 0x90)  // LD
      return std::string(" ") + RegNames[reg()] + std::string(", ") + PtrNames[ptr()] + suffix;
    else if (op() == 0xC0)  // ST
      return std::string(" ") + PtrNames[ptr()] + suffix + std::string(", ") + RegNames[reg()];
    else
      return "error";
  }
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|    IN |  DST |  PORT   |Z|z|I|i| A0 1010 0000| |
//|   OUT |  SRC |  PORT   |O|o|X|x| B0 1011 0000| |
class PortCmd: public Cmd {
 public:
  PortCmd(uint16_t cmd): Cmd(cmd) {}

  uint8_t port() { return (cmd_ >> 5) & 0x1F; }
  uint8_t reg() { return (cmd_ >> 9) & 0x07; }

  virtual std::string Params() {
    if (op() == 0xA0)
      return std::string(" ") + RegNames[reg()] + std::string(", PINS") + std::to_string(port());
    else if (op() == 0xB0)
      return " PORT" + std::to_string(port()) + ", " + RegNames[reg()];
    else
      return "error";
  }
};
