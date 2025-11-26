#pragma once

#include <map>
#include <stack>
#include <string>
#include <cstdint>
#include <vector>

#include "flags.h"

class CPU {
 public:
  CPU() {}
  CPU(std::vector<uint16_t> cmds): ROM(cmds) {}

  void Step(uint16_t cmd, uint16_t &ip);
  void Run(bool dbg);

  void PrintFlags();
  void PrintRegs();
  void PrintPorts();
  void PrintStack();

  void SyncFlags(uint8_t port);

  uint16_t GetPair(uint8_t idx);
  void SetPair(uint8_t idx, uint16_t val);
  uint16_t IncPair(uint8_t idx);
  uint16_t DecPair(uint8_t idx);

  uint8_t *ActiveRegsBank();  // current bank of registers
  const char** ActiveRegsNames();
  bool IsBank1Active();

  void FindDebugInfo();

  uint8_t RAM[65536];
  uint8_t PORTS[32];
  uint8_t PINS[32];
  bool Flags[flags::CNT] {};
  uint8_t RegsBank0[8] {};
  uint8_t RegsBank1[8] {};

  bool Stop {false};
  std::stack<uint16_t> Stack;
  std::vector<uint16_t> ROM;
  // Debug info from source asm file
  std::map<std::string, uint16_t> name_to_address_ {};
};
