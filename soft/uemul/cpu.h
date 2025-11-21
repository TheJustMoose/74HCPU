#pragma once

#include <stack>
#include <stdint.h>

#include "flags.h"

class CPU {
 public:
  void Step(uint16_t cmd, uint16_t &ip);
  void Run(bool dbg);

  void PrintFlags();
  void PrintRegs();
  void PrintPorts();
  void PrintStack();

  uint16_t GetPair(uint8_t idx);
  uint16_t GetPtr(uint8_t ptr);

  uint8_t *ActiveRegsBank();  // current bank of registers

  uint8_t RAM[65536];
  uint8_t PORTS[32];
  uint8_t PINS[32];
  bool Flags[flags::CNT];
  uint8_t RegsBank0[8];
  uint8_t RegsBank1[8];

  bool Stop {false};
  std::stack<uint16_t> Stack;
};
