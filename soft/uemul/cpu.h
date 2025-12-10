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
  CPU(std::vector<uint16_t> cmds): rom(cmds) {}

  void Step(uint16_t cmd, uint16_t &ip);
  void Run(bool dbg);

  void PrintFlags();
  void PrintRegs();
  void PrintPorts();
  void PrintStack();
  void PrintLabels(uint16_t ip);

  void SyncFlags(uint8_t port);

  uint16_t GetPair(uint8_t idx);
  void SetPair(uint8_t idx, uint16_t val);
  uint16_t IncPair(uint8_t idx);
  uint16_t DecPair(uint8_t idx);

  uint8_t *ActiveRegsBank();  // current bank of registers
  const char** ActiveRegsNames();
  bool IsBank1Active();

  bool FindDebugInfo(size_t& data_start);
  void ReadDebugInfo(size_t data_start);

  uint8_t ram[65536];
  uint8_t ports[32];
  uint8_t pins[32];
  bool flags[flags::CNT] {};
  uint8_t regs_bank0[8] {};
  uint8_t regs_bank1[8] {};

  bool stop {false};
  std::stack<uint16_t> call_stack;
  std::vector<uint16_t> rom;
  // Debug info from source asm file
  std::map<std::string, uint16_t> name_to_address {};
};
