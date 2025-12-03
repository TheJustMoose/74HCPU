#pragma once

#include <cstdint>
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

  uint8_t Dst() const { return (cmd_ >> 9) & 0x07; }
  bool IsConst() const { return (cmd_ >> 8) & 0x01; }
  uint8_t Src() const { return (cmd_ >> 5) & 0x07; }
  uint8_t Const() const { return (cmd_ & 0xFF); }

  bool NeedToZeroHighNibble() const { return cmd_ & 0x08; }
  bool NeedToZeroLowNibble() const { return cmd_ & 0x04; }

  uint8_t DstLowReg() const { return Dst() & 0x06; }     // low byte register
  uint8_t DstHighReg() const { return DstLowReg() | 1; } // high byte register

  uint8_t DstVal();
  uint8_t SrcVal();

  virtual void Execute();

  virtual uint8_t Calculate(uint8_t d, uint8_t s) { return 0; }

  std::string Params() override;
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

  virtual void Execute() {}
  std::string Params() override;
};

class InvCmd: public UnaryCmd {
 public:
  InvCmd(uint16_t cmd, CPU* cpu): UnaryCmd(cmd, cpu) {}

  void Execute() override;
};

class SwapCmd: public UnaryCmd {
 public:
  SwapCmd(uint16_t cmd, CPU* cpu): UnaryCmd(cmd, cpu) {}

  void Execute() override;
};

class LsrCmd: public UnaryCmd {
 public:
  LsrCmd(uint16_t cmd, CPU* cpu): UnaryCmd(cmd, cpu) {}

  void Execute() override;
};

class LsrcCmd: public UnaryCmd {
 public:
  LsrcCmd(uint16_t cmd, CPU* cpu): UnaryCmd(cmd, cpu) {}

  void Execute() override;
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

//|   LPM |  DST |W|EXT|D|U|OFFSET4| 80 1000 0000| |
class LpmCmd: public Cmd {
 public:
  LpmCmd(uint16_t cmd, CPU* cpu): Cmd(cmd, cpu) {}

  uint8_t Dst() { return (cmd_ >> 9) & 0x07; }
  uint8_t W() { return (cmd_ >> 8) & 0x01; }
  uint8_t Ptr() { return (cmd_ >> 6) & 0x03; }
  uint8_t AutoDec() { return (cmd_ >> 5) & 0x01; }
  uint8_t AutoInc() { return (cmd_ >> 4) & 0x01; }
  // I'm not sure about offset cause ADDER logic is on the RAM pcb only

  uint8_t DstLowReg() { return Dst() & 0x06; }     // low byte register
  uint8_t DstHighReg() { return DstLowReg() | 1; } // high byte register

  void Execute();
};

//     |F E D C  B A 9 8 7 6 5 4 3 2 1 0|
//| A0 |    IN |  DST |  PORT   |Z|z|I|i| |
class InputPortCmd: public Cmd {
 public:
  InputPortCmd(uint16_t cmd, CPU* cpu): Cmd(cmd, cpu) {}

  uint8_t Port() { return (cmd_ >> 4) & 0x1F; }
  uint8_t Reg() { return (cmd_ >> 9) & 0x07; }

  std::string Params() override;
  void Execute();
};

//     |F E D C  B A 9 8 7 6 5 4 3 2 1 0|
//|              2 1 0         4 3
//| B0 |   OUT | PORT |C| SRC |PRT|X|O|o| |
class OutputPortCmd: public Cmd {
 public:
  OutputPortCmd(uint16_t cmd, CPU* cpu): Cmd(cmd, cpu) {}

  uint8_t PortLo() { return (cmd_ >> 9) & 0x07; }
  bool IsConst() { return (cmd_ >> 8) & 0x01; }
  uint8_t Reg() { return (cmd_ >> 5) & 0x07; }
  uint8_t PortHi() { return (cmd_ >> 3) & 0x03; }
  uint8_t Const() { return (cmd_ & 0xFF); }

  uint8_t Port() { return PortLo() | (IsConst() ? 0 : (PortHi() << 3)); }
  uint8_t SrcVal();

  std::string Params() override;
  void Execute();
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|   CMP |  DST |C| SRC |-|   -   | D0 1101 0000|+|
//|  CMPC |  DST |C| SRC |-|   -   | E0 1110 0000|+|
class CmpCmd: public Cmd {
 public:
  CmpCmd(uint16_t cmd, CPU* cpu): Cmd(cmd, cpu) {}

  uint8_t Dst() { return (cmd_ >> 9) & 0x07; }
  bool IsConst() { return (cmd_ >> 8) & 0x01; }
  uint8_t Src() { return (cmd_ >> 5) & 0x07; }
  uint8_t Const() { return (cmd_ & 0xFF); }

  uint8_t DstVal();
  uint8_t SrcVal();

  virtual void Execute();

  std::string Params() override;
};

class CmpcCmd: public CmpCmd {
 public:
  CmpcCmd(uint16_t cmd, CPU* cpu): CmpCmd(cmd, cpu) {}

  virtual void Execute();
};
