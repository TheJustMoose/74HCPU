#include "cpu.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdio>

#include "commands.h"
#include "cpu.h"
#include "flags.h"
#include "names.h"
#include "offset2int.h"

using namespace std;

const char* Op(uint16_t cmd) {
  uint8_t op = (cmd >> 8) & 0xFF;
  if (cmd == 0xFFFE)
    return StopName;
  else if ((op & 0xF0) == 0xF0)
    return BranchNames[op & 0x0F];
  else
    return OpNames[op >> 4];
}

uint16_t CPU::GetPair(uint8_t idx) {
  if (idx > 3) {
    cout << "Error pointer register number: " << static_cast<int>(idx) << endl;
    return 0;
  }
  uint16_t res = regs_bank1[idx*2 + 1];
  res <<= 8;
  res += regs_bank1[idx*2];
  return res;
}

void CPU::SetPair(uint8_t idx, uint16_t val) {
  if (idx > 3) {
    cout << "Error pointer register number: " << static_cast<int>(idx) << endl;
    return;
  }
  regs_bank1[idx*2 + 1] = (val >> 8);
  regs_bank1[idx*2] = val & 0xFF;
}

uint16_t CPU::IncPair(uint8_t idx) {
  uint16_t res = GetPair(idx);
  res++;
  SetPair(idx, res);
  return res;
}

uint16_t CPU::DecPair(uint8_t idx) {
  uint16_t res = GetPair(idx);
  res--;
  SetPair(idx, res);
  return res;
}

uint8_t *CPU::ActiveRegsBank() { // current bank of registers
  return IsBank1Active() ? regs_bank1 : regs_bank0;
}

const char** CPU::ActiveRegsNames() {
  return IsBank1Active() ? PtrRegNames : RegNames;
}

bool CPU::IsBank1Active() {
  return flags[flags::BF];
}

void CPU::PrintFlags() {
  cout << "Flags: ";
  for (int i = 0; i < flags::CNT; i++)
    cout << (flags[i] ? FlagNames[i] : "--") << " ";
}

void CPU::PrintRegs() {
  cout << "Regs: ";
  for (int i = 0; i < 8; i++)
    cout << hex << setw(2) << (uint16_t)regs_bank0[i] << " ";

  cout << " Ptrs: " << setfill('0');
  cout << hex << setw(4) << GetPair(0) << " ";
  cout << hex << setw(4) << GetPair(1) << " ";
  cout << hex << setw(4) << GetPair(2) << " ";
  cout << hex << setw(4) << GetPair(3) << " ";

  PrintFlags();

  cout << endl << endl;
}

void CPU::PrintPorts() {
  cout << "Ports: ";
  for (int i = 0; i < 16; i++)
    cout << hex << setw(2) << (uint16_t)ports[i] << " ";

  cout << endl << endl;
}

void CPU::PrintStack() {
  cout << "Stack: ";
  if (call_stack.empty()) {
    cout << "empty" << endl << endl;
    return;
  }

  stack<uint16_t> cpy = call_stack;
  while (!cpy.empty()) {
    cout << hex << setw(4) << cpy.top() << " ";
    cpy.pop();
  }

  cout << endl << endl;
}

void CPU::PrintLabels(uint16_t ip) {
  auto it = NameToAddress.begin();
  bool found {false};
  while (it != NameToAddress.end()) {
    if (it->second == ip) {
      cout << it->first << ":";
      found = true;
    }
    it++;
  }

  if (found)
    cout << endl;
}

void CPU::SyncFlags(uint8_t port) {
  const uint8_t CPU_FLAGS = 6;
  if (port == CPU_FLAGS)
    flags[flags::BF] = ports[CPU_FLAGS] & 0x40;
}

string BranchAddr(uint16_t cmd, uint16_t ip) {
  cmd &= 0xFF;  // 8 low bits have offset value
  uint16_t branch_addr = ip + ByteOffsetToInt(static_cast<uint8_t>(cmd));
  stringstream ss;
  ss << hex << branch_addr;
  string res = ss.str();
  while (res.size() < 4)
    res = "0" + res;
  return res;
}

void CPU::Step(uint16_t cmd, uint16_t &ip) {
  uint8_t op = (cmd >> 8) & 0xFF;
  if ((cmd & 0xF000) == 0xF000)  // branches
    ;
  else           // arithm instructions
    op &= 0xF0;  // have to clean low bits

  PrintLabels(ip);
  cout << "IP: " << hex << setw(4) << ip
       << ", cmd: " << cmd << ", " << Op(cmd);

  if (op == 0x00) {  // ADD
    AddCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    acmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x10) {  // ADDC
    AddcCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    acmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x20) {  // AND
    AndCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    acmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x30) {  // OR
    OrCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    acmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x40) {  // XOR
    XorCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    acmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x50) {  // MUL
    MulCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    acmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x60) {  // UNO
    string params;
    switch ((cmd >> 5) & 0x03) {
      case 0: { InvCmd icmd(cmd, this); params = icmd.Params(); icmd.Execute(); } break;
      case 1: { SwapCmd scmd(cmd, this); params = scmd.Params(); scmd.Execute(); } break;
      case 2: { LsrCmd lcmd(cmd, this); params = lcmd.Params(); lcmd.Execute(); } break;
      case 3: { LsrcCmd lcmd(cmd, this); params = lcmd.Params(); lcmd.Execute(); } break;
      default: cout << "Unknown op for this switch: " << op << endl; break;
    }
    cout << params << "  -->  ";
    PrintRegs();
    ip++;
  } else if (op == 0x70) {  // MOV
    MovCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    acmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x80) {  // LPM
    LpmCmd lcmd(cmd, this);
    cout << lcmd.Params() << "  -->  ";
    lcmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0x90) {  // LD
    LoadFromMemoryCmd mcmd(cmd, this);
    cout << mcmd.Params() << "  -->  ";
    mcmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0xA0) {  // IN
    InputPortCmd pcmd(cmd, this);
    cout << pcmd.Params() << "  -->  ";
    pcmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0xB0) {  // OUT
    OutputPortCmd pcmd(cmd, this);
    cout << pcmd.Params() << "  -->  ";
    pcmd.Execute();
    SyncFlags(pcmd.Port());
    PrintPorts();
    ip++;
  } else if (op == 0xC0) {  // ST
    StoreToMemoryCmd mcmd(cmd, this);
    cout << mcmd.Params() << "  -->  ";
    mcmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0xD0) {  // CMP
    CmpCmd ccmd(cmd, this);
    cout << ccmd.Params() << "  -->  ";
    ccmd.Execute();
    PrintRegs();
    ip++;
  } else if (op == 0xE0) {  // CMPC
    CmpcCmd ccmd(cmd, this);
    cout << ccmd.Params() << "  -->  ";
    ccmd.Execute();
    PrintRegs();
    ip++;
  } else if (op >= 0xF0 && op <= 0xFF) {  // BRANCH
    int16_t offset = ByteOffsetToInt(cmd & 0xFF);
    if (op == 0xF2 || op == 0xF3 || op == 0xFF)
      cout << endl;
    else
      cout << " " << BranchAddr(cmd, ip) << ", offs: " << offset << endl;
    switch (op) {
      case 0xF0: call_stack.push(ip + 1); ip += offset; break;                 // CALL
      case 0xF1: ip += offset; break;                                     // JMP
      case 0xF2: ip = call_stack.top(); call_stack.pop(); break;                    // RET
      case 0xF3: ip = call_stack.top(); call_stack.pop(); break;                    // RETI
      case 0xF4: if (flags[flags::LF]) ip += offset; else ip++; break;    // JL
      case 0xF5: if (flags[flags::EF]) ip += offset; else ip++; break;    // JE
      case 0xF6: if (!flags[flags::EF]) ip += offset; else ip++; break;   // JNE
      case 0xF7: if (flags[flags::ZF]) ip += offset; else ip++; break;    // JG
      case 0xF8: if (flags[flags::ZF]) ip += offset; else ip++; break;    // JZ
      case 0xF9: if (!flags[flags::ZF]) ip += offset; else ip++; break;   // JNZ
      case 0xFA: if (flags[flags::CF]) ip += offset; else ip++; break;    // JC
      case 0xFB: if (!flags[flags::CF]) ip += offset; else ip++; break;   // JNC
      case 0xFC: if (flags[flags::HCF]) ip += offset; else ip++; break;   // JHC
      case 0xFD: if (!flags[flags::HCF]) ip += offset; else ip++; break;  // JHNC
      case 0xFE: call_stack.push(ip + 1); ip = offset << 8; break;             // AFCALL
      case 0xFF: ip++; if ((offset & 0x01) == 0) stop = true; break;      // NOP/STOP
    }
    cout << "new ip: " << hex << setw(4) << ip << endl;
    PrintStack();
  } else {  // it's impossible I hope
    cout << "WTF?! Error opcode: " << op << endl;
  }
}

void CPU::Run(bool dbg) {
  size_t start {0};
  if (FindDebugInfo(start))
    ReadDebugInfo(start);

  PrintRegs();
  for (uint16_t ip = 0; ip < rom.size() && !stop;) {
    Step(rom[ip], ip);
    if (dbg)
      getchar();
  }
}

bool CPU::FindDebugInfo(size_t& data_start) {
  // Debug Info should have at least 8 chars mark:
  // "\0\0\0\0DBGI"
  if (rom.size() < 8)
    return false;

  cout << "ROM size: " << rom.size() << endl;

  bool found {false};
  data_start = 0;
  for (size_t i = 0; i < rom.size() - 8; i++) {
    if (rom[i] == 0 && rom[i + 1] == 0 && rom[i + 2] == 0 && rom[i + 3] == 0 &&
        rom[i + 4] == 'D' && rom[i + 5] == 'B' && rom[i + 6] == 'G' && rom[i + 7] == 'I') {
      data_start = i + 8;
      found = true;
      break;
    }
  }

  cout << "Debug Info start: " << (found ? static_cast<int>(data_start) : -1) << endl;
  return found;
}

void CPU::ReadDebugInfo(size_t data_start) {
  while (true) {
    uint16_t addr{0};
    if (data_start < rom.size())
      addr = rom[data_start++];
    else
      break;

    string lbl;
    while (data_start < rom.size()) {
      if (!rom[data_start]) {
        data_start++;
        break;
      }
      lbl += static_cast<char>(rom[data_start]);
      data_start++;
    }
    if (!lbl.size())
      break;
    // label was found
    NameToAddress[lbl] = addr;
    cout << hex << addr << " " << lbl << endl;
  }
}
