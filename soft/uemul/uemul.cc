#include <conio.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stack>
#include <vector>

#include "commands.h"
#include "names.h"
#include "offset2int.h"
#include "read_hex.h"

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

vector<uint16_t> Cmds {
  0x7140,  // MOV R0, 0x40  ; BF = 1
  0xB0A0,  // out FLAGS, R0 ; FLAGS = 10; 1011 000 01010 0000 ; B0A0
  0x7DFF,  // MOV SPL, 0xFF
  0x7F7F,  // MOV SPH, 0x7F
  0x7122,  // MOV R0, 22h
  0x7F33,  // MOV R7, 33h
  0xFFFE,
  0xFD00
};

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

  uint8_t *ActiveRegsBank() { // current bank of registers
    return Flags[BF] ? RegsBank1 : RegsBank0;
  }

  uint8_t RAM[65536];
  uint8_t PORTS[32];
  uint8_t PINS[32];
  bool Flags[FLAGS_CNT];
  uint8_t RegsBank0[8];
  uint8_t RegsBank1[8];

  bool Stop { false };
  stack<uint16_t> Stack;
};

CPU cpu;

uint16_t CPU::GetPair(uint8_t idx) {
  uint16_t res = RegsBank1[idx*2 + 1];
  res <<= 8;
  res += RegsBank1[idx*2];
  return res;
}

uint16_t CPU::GetPtr(uint8_t ptr) {
  if (ptr >= 0 && ptr <= 3)
    return GetPair(ptr);
  else {
    cout << "Error pointer register number: " << ptr << endl;
    return 0;
  }
}

void CPU::PrintFlags() {
  cout << "Flags: ";
  for (int i = 0; i < FLAGS_CNT; i++)
    cout << (Flags[i] ? FlagNames[i] : "--") << " ";
}

void CPU::PrintRegs() {
  cout << "Regs: ";
  for (int i = 0; i < 8; i++)
    cout << hex << setw(2) << (uint16_t)RegsBank0[i] << " ";

  cout << " Ptrs: " << setfill('0');
  cout << hex << setw(4) << GetPtr(3) << " ";
  cout << hex << setw(4) << GetPtr(2) << " ";
  cout << hex << setw(4) << GetPtr(1) << " ";
  cout << hex << setw(4) << GetPtr(0) << " ";

  PrintFlags();

  cout << endl << endl;
}

void CPU::PrintPorts() {
  cout << "Ports: ";
  for (int i = 0; i < 8; i++)
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

void SyncFlags(uint8_t port) {
  if (port == 10)
    cpu.Flags[BF] = cpu.PORTS[10] & 0x40;
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

  if (op == 0x60) {  // UNO
    UnaryCmd ucmd(cmd);
    cout << " " << ucmd.Params() << "  -->  ";
    uint8_t rsrc = ActiveRegsBank()[ucmd.dst()];
    uint8_t rdst = 0;
    bool cf = Flags[CF];
    switch (ucmd.type()) {
      case 0: rdst = rsrc ^ 0xFF; break;  // INV
      case 1: rdst = ((rsrc & 0x0F) << 4) + ((rsrc & 0xF0) >> 4); break;  // SWAP
      case 2: Flags[CF] = rsrc & 0x01; rdst = rsrc >> 1; break;  // LSR
      case 3: Flags[CF] = rsrc & 0x01; rdst = rsrc >> 1; rdst |= (cf ? 0x80 : 0x00); break;  // LSRC
      default: cout << "Unknown op for this switch: " << op << endl; break;
    }
    ActiveRegsBank()[ucmd.dst()] = rdst;
    PrintRegs();
    ip++;
  } else if (op >= 0x00 && op <= 0x80) {  // ARITHM
    ArithmCmd acmd(cmd);
    cout << acmd.Params() << "  -->  ";
    uint8_t dst_val = ActiveRegsBank()[acmd.dst()];
    uint8_t src_val = acmd.is_cnst() ? acmd.cnst() : ActiveRegsBank()[acmd.src()];
    switch (op) {
      case 0x00: Flags[CF] = (dst_val + src_val > 255); dst_val += src_val; break;  // ADD // TODO: add sub operation here
      case 0x10: Flags[CF] = (dst_val + src_val > 255); dst_val += src_val + Flags[CF]; break;  // ADDC
      case 0x20: dst_val &= src_val; break;  // AND
      case 0x30: dst_val |= src_val; break;  // OR
      case 0x40: dst_val ^= src_val; break;  // XOR
      case 0x50: dst_val *= src_val; break;  // MUL  // TODO: have to write into register pair
      // 0x60 is UNO op
      case 0x70: dst_val = src_val; break;   // MOV
      case 0x80: break;  // LPM operation must be here
      default: cout << "Unknown op for this switch: " << op << endl; break;
    }
    if (op != 0x70)
      Flags[ZF] = (dst_val == 0);
    ActiveRegsBank()[acmd.dst()] = dst_val;
    PrintRegs();
    ip++;
  } else if (op == 0x90) {  // LD
    MemoryCmd mcmd(cmd);
    cout << mcmd.Params() << "  -->  ";
    uint8_t val = RAM[GetPtr(mcmd.ptr()) + (op & 0x0F)];  // TODO: replace it to mcmd.offs()
    ActiveRegsBank()[mcmd.reg()] = val;
    PrintRegs();
    ip++;
  } else if (op == 0xA0) {  // IN
    PortCmd pcmd(cmd);
    cout << pcmd.Params() << "  -->  ";
    uint8_t rdst = PINS[(op >> 4) & 0x1F];
    ActiveRegsBank()[pcmd.reg()] = rdst;
    PrintRegs();
    ip++;
  } else if (op == 0xB0) {  // OUT
    PortCmd pcmd(cmd);
    cout << pcmd.Params() << "  -->  ";
    PORTS[pcmd.port()] = ActiveRegsBank()[pcmd.reg()];
    SyncFlags(pcmd.port());
    PrintPorts();
    ip++;
  } else if (op == 0xC0) {  // ST
    MemoryCmd mcmd(cmd);
    cout << mcmd.Params() << "  -->  ";
    uint8_t val = ActiveRegsBank()[mcmd.reg()];
    RAM[GetPtr(mcmd.ptr()) + (op & 0x0F)] = val;
    PrintRegs();
    ip++;
  } else if (op == 0xD0 || op == 0xE0) {  // CMP
    // CMP implementation, CPMC has not implemented yet
    ArithmCmd acmd(cmd);
    cout << acmd.Params() << "  -->  ";
    uint8_t rdst = ActiveRegsBank()[acmd.dst()];
    uint8_t rsrc = acmd.is_cnst() ? acmd.cnst() : ActiveRegsBank()[acmd.src()];
    if (rdst < rsrc) {
      Flags[LF] = true; Flags[EF] = false; Flags[GF] = false;
    } else if (rdst == rsrc) {
      Flags[LF] = false; Flags[EF] = true; Flags[GF] = false;
    } else if (rdst > rsrc) {
      Flags[LF] = false; Flags[EF] = false; Flags[GF] = true;
    }
    ip++;
  } else if (op >= 0xF0 && op <= 0xFF) {  // BRANCH
    int16_t offset = ByteOffsetToInt(cmd & 0xFF);
    if (op == 0xF2 || op == 0xF3 || op == 0xFF)
      cout << endl;
    else
      cout << " " << BranchAddr(cmd, ip) << ", offs: " << offset << endl;
    switch (op) {
      case 0xF0: Stack.push(ip + 1); ip += offset; break;            // CALL
      case 0xF1: ip += offset; break;                                // JMP
      case 0xF2: ip = Stack.top(); Stack.pop(); break;               // RET
      case 0xF3: ip = Stack.top(); Stack.pop(); break;               // RETI
      case 0xF4: if (Flags[LF]) ip += offset; else ip++; break;      // JL
      case 0xF5: if (Flags[EF]) ip += offset; else ip++; break;      // JE
      case 0xF6: if (!Flags[EF]) ip += offset; else ip++; break;     // JNE
      case 0xF7: if (Flags[ZF]) ip += offset; else ip++; break;      // JG
      case 0xF8: if (Flags[ZF]) ip += offset; else ip++; break;      // JZ
      case 0xF9: if (!Flags[ZF]) ip += offset; else ip++; break;     // JNZ
      case 0xFA: if (Flags[CF]) ip += offset; else ip++; break;      // JC
      case 0xFB: if (!Flags[CF]) ip += offset; else ip++; break;     // JNC
      case 0xFC: if (Flags[HCF]) ip += offset; else ip++; break;     // JHC
      case 0xFD: if (!Flags[HCF]) ip += offset; else ip++; break;    // JHNC
      case 0xFE: Stack.push(ip + 1); ip = offset << 8; break;        // AFCALL
      case 0xFF: ip++; if ((offset & 0x01) == 0) Stop = true; break; // NOP/STOP
    }
    cout << "new ip: " << hex << setw(4) << ip << endl;
    PrintStack();
  } else {  // it's impossible I hope
    cout << "WTF?! Error opcode: " << op << endl;
  }
}

void CPU::Run(bool dbg) {
  PrintRegs();
  for (uint16_t ip = 0; ip < Cmds.size() && !Stop;) {
    Step(Cmds[ip], ip);
    if (dbg)
      _getch();
  }
}

int main(int argc, char* argv[]) {
  // TODO: fix flags storing

  if (sizeof(uint16_t) != 2) {
    cout << "Strange system parameters detected. Word size should be 16 bits." << endl;
    return 1;
  }

  if (argc >= 2 && argv[1]) {
    string fname(argv[1]);
    if (!ReadHex(fname, Cmds))
      return 1;
  }

  bool dbg = false;
  if (argc >= 3 && argv[2]) {
    string a(argv[2]);
    dbg = (a == "-dbg" || a == "--debug");
  }

  cpu.Run(dbg);

  return 0;
}
