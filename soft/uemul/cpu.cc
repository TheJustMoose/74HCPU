#include "cpu.h"

#include <conio.h>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "commands.h"
#include "compare.h"
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
  uint16_t res = RegsBank1[idx*2 + 1];
  res <<= 8;
  res += RegsBank1[idx*2];
  return res;
}

void CPU::SetPair(uint8_t idx, uint16_t val) {
  if (idx > 3) {
    cout << "Error pointer register number: " << static_cast<int>(idx) << endl;
    return;
  }
  RegsBank1[idx*2 + 1] = (val >> 8);
  RegsBank1[idx*2] = val & 0xFF;
}

uint16_t CPU::IncPair(uint8_t idx) {
  uint16_t res = GetPair(idx);
  res++;
  SetPair(idx, res);
  return res;
}

uint8_t *CPU::ActiveRegsBank() { // current bank of registers
  return Flags[flags::BF] ? RegsBank1 : RegsBank0;
}

void CPU::PrintFlags() {
  cout << "Flags: ";
  for (int i = 0; i < flags::CNT; i++)
    cout << (Flags[i] ? FlagNames[i] : "--") << " ";
}

void CPU::PrintRegs() {
  cout << "Regs: ";
  for (int i = 0; i < 8; i++)
    cout << hex << setw(2) << (uint16_t)RegsBank0[i] << " ";

  cout << " Ptrs: " << setfill('0');
  cout << hex << setw(4) << GetPair(3) << " ";
  cout << hex << setw(4) << GetPair(2) << " ";
  cout << hex << setw(4) << GetPair(1) << " ";
  cout << hex << setw(4) << GetPair(0) << " ";

  PrintFlags();

  cout << endl << endl;
}

void CPU::PrintPorts() {
  cout << "Ports: ";
  for (int i = 0; i < 16; i++)
    cout << hex << setw(2) << (uint16_t)PORTS[i] << " ";

  cout << endl << endl;
}

void CPU::PrintStack() {
  cout << "Stack: ";
  if (Stack.empty()) {
    cout << "empty" << endl << endl;
    return;
  }

  stack<uint16_t> cpy = Stack;
  while (!cpy.empty()) {
    cout << hex << setw(4) << cpy.top() << " ";
    cpy.pop();
  }

  cout << endl << endl;
}

void CPU::SyncFlags(uint8_t port) {
  if (port == 10)
    Flags[flags::BF] = PORTS[10] & 0x40;
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
    UnaryCmd ucmd(cmd, this);
    cout << " " << ucmd.Params() << "  -->  ";
    uint8_t rsrc = ucmd.DstVal();
    uint8_t rdst = 0;
    bool cf = Flags[flags::CF];
    switch (ucmd.Type()) {
      case 0: rdst = rsrc ^ 0xFF; break;  // INV
      case 1: rdst = ((rsrc & 0x0F) << 4) + ((rsrc & 0xF0) >> 4); break;  // SWAP
      case 2: Flags[flags::CF] = rsrc & 0x01; rdst = rsrc >> 1; break;  // LSR
      case 3: Flags[flags::CF] = rsrc & 0x01; rdst = rsrc >> 1; rdst |= (cf ? 0x80 : 0x00); break;  // LSRC
      default: cout << "Unknown op for this switch: " << op << endl; break;
    }
    ActiveRegsBank()[ucmd.Dst()] = rdst;
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
  } else if (op == 0xD0 || op == 0xE0) {  // CMP
    // TODO: CMP implementation, CMPC has not implemented yet
    ArithmCmd acmd(cmd, this);
    cout << acmd.Params() << "  -->  ";
    auto [lf, ef, gf] = Compare(acmd.DstVal(), acmd.SrcVal());
    Flags[flags::LF] = lf;
    Flags[flags::EF] = ef;
    Flags[flags::GF] = gf;
    PrintRegs();
    ip++;
  } else if (op >= 0xF0 && op <= 0xFF) {  // BRANCH
    int16_t offset = ByteOffsetToInt(cmd & 0xFF);
    if (op == 0xF2 || op == 0xF3 || op == 0xFF)
      cout << endl;
    else
      cout << " " << BranchAddr(cmd, ip) << ", offs: " << offset << endl;
    switch (op) {
      case 0xF0: Stack.push(ip + 1); ip += offset; break;                 // CALL
      case 0xF1: ip += offset; break;                                     // JMP
      case 0xF2: ip = Stack.top(); Stack.pop(); break;                    // RET
      case 0xF3: ip = Stack.top(); Stack.pop(); break;                    // RETI
      case 0xF4: if (Flags[flags::LF]) ip += offset; else ip++; break;    // JL
      case 0xF5: if (Flags[flags::EF]) ip += offset; else ip++; break;    // JE
      case 0xF6: if (!Flags[flags::EF]) ip += offset; else ip++; break;   // JNE
      case 0xF7: if (Flags[flags::ZF]) ip += offset; else ip++; break;    // JG
      case 0xF8: if (Flags[flags::ZF]) ip += offset; else ip++; break;    // JZ
      case 0xF9: if (!Flags[flags::ZF]) ip += offset; else ip++; break;   // JNZ
      case 0xFA: if (Flags[flags::CF]) ip += offset; else ip++; break;    // JC
      case 0xFB: if (!Flags[flags::CF]) ip += offset; else ip++; break;   // JNC
      case 0xFC: if (Flags[flags::HCF]) ip += offset; else ip++; break;   // JHC
      case 0xFD: if (!Flags[flags::HCF]) ip += offset; else ip++; break;  // JHNC
      case 0xFE: Stack.push(ip + 1); ip = offset << 8; break;             // AFCALL
      case 0xFF: ip++; if ((offset & 0x01) == 0) Stop = true; break;      // NOP/STOP
    }
    cout << "new ip: " << hex << setw(4) << ip << endl;
    PrintStack();
  } else {  // it's impossible I hope
    cout << "WTF?! Error opcode: " << op << endl;
  }
}

void CPU::Run(bool dbg) {
  PrintRegs();
  for (uint16_t ip = 0; ip < ROM.size() && !Stop;) {
    Step(ROM[ip], ip);
    if (dbg)
      _getch();
  }
}
