#pragma once

#include <stdint.h>
#include <string>

#include "names.h"

class CPU;

class Cmd {
 public:
  Cmd(uint16_t cmd, CPU* cpu = nullptr): cmd_(cmd), cpu_(cpu) {}

  uint16_t CmdCode() { return cmd_; }
  uint8_t OpCode() { return (cmd_ >> 8) & 0xF0; }

  virtual std::string Params() { return ""; }

 protected:
  uint16_t cmd_ {0};
  CPU* cpu_ {nullptr};
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|   ADD |  DST |C| SRC |-|Z|z|I|i|
class ArithmCmd: public Cmd {
 public:
  ArithmCmd(uint16_t cmd, CPU* cpu): Cmd(cmd, cpu) {}

  uint8_t Dst() { return (cmd_ >> 9) & 0x07; }
  bool IsConst() { return (cmd_ >> 8) & 0x01; }
  uint8_t Src() { return (cmd_ >> 5) & 0x07; }
  uint8_t Const() { return (cmd_ & 0xFF); }

  uint8_t DstVal();
  uint8_t SrcVal();

  virtual void Execute();

  virtual uint8_t Calculate(uint8_t d, uint8_t s) { return 0; }

  std::string Params() override;

  uint8_t res() { return res_; }
  bool CF() { return CF_; };
  bool ZF() { return ZF_; };

 protected:
  uint8_t res_ {0};
  bool CF_ {false};
  bool ZF_ {false};
};

class AddCmd: public ArithmCmd {
 public:
  AddCmd(uint16_t cmd, CPU* cpu): ArithmCmd(cmd, cpu) {}

  uint8_t Calculate(uint8_t d, uint8_t s) override;
};

class AddcCmd: public ArithmCmd {
 public:
  AddcCmd(uint16_t cmd, CPU* cpu): ArithmCmd(cmd, cpu) {}

  uint8_t Calculate(uint8_t d, uint8_t s) override;
};

class AndCmd: public ArithmCmd {
 public:
  AndCmd(uint16_t cmd, CPU* cpu): ArithmCmd(cmd, cpu) {}

  uint8_t Calculate(uint8_t d, uint8_t s) override;
};

class OrCmd: public ArithmCmd {
 public:
  OrCmd(uint16_t cmd, CPU* cpu): ArithmCmd(cmd, cpu) {}

  uint8_t Calculate(uint8_t d, uint8_t s) override;
};

class XorCmd: public ArithmCmd {
 public:
  XorCmd(uint16_t cmd, CPU* cpu): ArithmCmd(cmd, cpu) {}

  uint8_t Calculate(uint8_t d, uint8_t s) override;
};

class MulCmd: public ArithmCmd {
 public:
  MulCmd(uint16_t cmd, CPU* cpu): ArithmCmd(cmd, cpu) {}

  void Execute() override;
};

class MovCmd: public ArithmCmd {
 public:
  MovCmd(uint16_t cmd, CPU* cpu): ArithmCmd(cmd, cpu) {}

  uint8_t Calculate(uint8_t d, uint8_t s) override;
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|   UNO |  DST |0|-|TYP|F|-|-|-|-| 60 0110 0000|*| унарные команды не используют операнд SRC, поэтому нельзя использовать инверторы и отдельные нибблы
class UnaryCmd: public Cmd {
 public:
  UnaryCmd(uint16_t cmd, CPU* cpu): Cmd(cmd, cpu) {}

  uint8_t Dst() { return (cmd_ >> 9) & 0x07; }
  uint8_t Type() { return (cmd_ >> 5) & 0x03; }

  uint8_t DstVal();

  std::string Params() override;
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|    LD |  DST |0|EXT|D|U|OFFSET4| 90 1001 0000| |
//|    ST |  SRC |0|EXT|D|U|OFFSET4| C0 1100 0000| |
class MemoryCmd: public Cmd {
 public:
  MemoryCmd(uint16_t cmd, CPU* cpu): Cmd(cmd, cpu) {}

  uint8_t Reg() { return (cmd_ >> 9) & 0x07; }
  uint8_t Ptr() { return (cmd_ >> 6) & 0x03; }
  uint8_t AutoDec() { return (cmd_ >> 5) & 0x01; }
  uint8_t AutoInc() { return (cmd_ >> 4) & 0x01; }
  uint8_t Offs() { return cmd_ & 0x0F; }

  virtual void Execute() {}

  std::string Suffix();
};

class LoadFromMemoryCmd: public MemoryCmd {
 public:
  LoadFromMemoryCmd(uint16_t cmd, CPU* cpu): MemoryCmd(cmd, cpu) {}

  std::string Params() override;
  void Execute() override;
};

class StoreToMemoryCmd: public MemoryCmd {
 public:
  StoreToMemoryCmd(uint16_t cmd, CPU* cpu): MemoryCmd(cmd, cpu) {}

  std::string Params() override;
  void Execute() override;
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|    IN |  DST |  PORT   |Z|z|I|i| A0 1010 0000| |
//|   OUT |  SRC |  PORT   |O|o|X|x| B0 1011 0000| |
class PortCmd: public Cmd {
 public:
  PortCmd(uint16_t cmd): Cmd(cmd) {}

  uint8_t Port() { return (cmd_ >> 5) & 0x1F; }
  uint8_t Reg() { return (cmd_ >> 9) & 0x07; }

  virtual void Execute() {}
};

class InputPortCmd: public PortCmd {
 public:
  InputPortCmd(uint16_t cmd): PortCmd(cmd) {}

  std::string Params() override;
  void Execute() override;
};

class OutputPortCmd: public PortCmd {
 public:
  OutputPortCmd(uint16_t cmd): PortCmd(cmd) {}

  std::string Params() override;
  void Execute() override;
};
