/*
|----------------------------------------------|f|
|   4   +   3 + 1 + 3  +    5    = 16          |l|
|   HIGH BYTE   |    LOW BYTE    |  HIGH BYTE  |a| * - меняем CF, HCF, ZF (результат сложения) == A-FLAGS (Arithm)
|0 1 2 3  4 5 6 7 8 9 A B C D E F|   (again)   |g| + - меняем LF, EQ, GF (результат сравнения) == C-FLAGS (Compare)
|----------------------------------------------|s|
|   ADD |  DST |C| SRC |-|Z|z|I|i| 00 0000 0000|*| ADDI  R0, 1    // INC R0;
|  ADDC |  DST |C| SRC |F|Z|z|I|i| 10 0001 0000|*| ADDCI R1, 0    // INCC R1;    // бит F нужен для команд -INC-, -DEC-, SUB, -INV-, NEG
|   AND |  DST |C| SRC |-|Z|z|I|i| 20 0010 0000|*| ANDI R2, 0x0F  // здесь есть свободный бит, и вместо F туда можно положить S[WAP] или S[HIFT]
|    OR |  DST |C| SRC |-|Z|z|I|i| 30 0011 0000|*| ORI R3, 2      // в любом случае, сдвиг старшего нибла на место младшего кажется полезным
|   XOR |  DST |C| SRC |-|Z|z|I|i| 40 0100 0000|*| XORI R4, 4     // кстати, в этом случае можно ещё и от команды SWAP избавиться
|   MUL |  DST |C| SRC |-|Z|z|I|i| 50 0101 0000|*| MULI R6, 10    // R7:R6 := R6*10
|   UNO |  DST |0|-|TYP|F|-|-|-|-| 60 0110 0000|*| унарные команды не используют операнд SRC, поэтому нельзя использовать инверторы и отдельные нибблы
|   MOV |  DST |C| SRC |*|Z|z|I|i| 70 0111 0000| | MOVI R5, 5     // LDI R5, 5       // * - можно использовать, чтобы пихать адрес в SPL/SPH (правда, только из других регистров), или для того, чтобы сделать MOVW
|   LPM |  DST |W|EXT|D|U|OFFSET4| 80 1000 0000| | LPM R0, [IX]   // LPM R1, [X+1]   // LPM R1, [DX-1]  // W - читать слово (1) или байт (0)
|    LD |  DST |0|EXT|D|U|OFFSET4| 90 1001 0000| | LD R1, [-Y]    // LD R3, [+SP-1] ; POP R3  // ноль на месте бита C можно использовать как пятый бит OFFSET-а
|    IN |  DST |  PORT   |Z|z|I|i| A0 1010 0000| | IN R2, PIN1    // IN R2, ~PIN1 ; Ii - инвертировать порт перед чтением, Zz - занулять ниббл при чтении (или сделать отдельный IMASK?)
|--------------|7-8-9-A-B|-------|-------------|-| всё что ниже - не модифицирует регистры
|   OUT |  SRC |  PORT   |O|o|X|x| B0 1011 0000| | OUT PORT0, R2  // XOUT PORT0, R2 (вместо Oo можно сделать вчетверо больше регистров!)
|    ST |  SRC |0|EXT|D|U|OFFSET4| C0 1100 0000| | ST [Y], R2     // ST [IX], R2     // ST [Y+2], R2   // ST [-SP], R2 ; PUSH R2
|   CMP |  DST |C| SRC |-|   -   | D0 1101 0000|+| CMPI R3, 10    // CMP  R0, R2
|  CMPC |  DST |C| SRC |-|   -   | E0 1110 0000|+| CMPCI R4, 10   // CMPC R1, R3
| BRNCH |  CALL  | 8 bit OFFSET  | F0 1111 0000| | переход в +-127 команды, значение прибавляется к текущему счётчику команд
| BRNCH |   JMP  | 8 bit OFFSET  | F1 1111 0001| | <- COP low nibble
| BRNCH |   RET  | 8 bit +STACK  | F2 1111 0010| | на сколько байт переместить указатель стека данных, дабы пропустить запушенные параметры
| BRNCH |  RETI  | 8 bit OFFSET  | F3 1111 0011| | RETI           // однако, у меня нет механизма для переставления SP на несколько байт за команду RET
| BRNCH |    JL  | 8 bit OFFSET  | F4 1111 0100| | if (v < w)     // поэтому вместо константы можно воткнуть сюда битик указания на RETI, вместо RET
| BRNCH |    JE  | 8 bit OFFSET  | F5 1111 0101| | if (v == w)    // а не правильней ли будет сделать JLE вдобавок к отдельным JL + JE ?
| BRNCH |   JNE  | 8 bit OFFSET  | F6 1111 0110| | if (v != w)
| BRNCH |    JG  | 8 bit OFFSET  | F7 1111 0111| | if (v > w)     // с другой стороны, эту инструкцию можно эмулировать программно
| BRNCH |    JZ  | 8 bit OFFSET  | F8 1111 1000| | if (v == 0)    // путём переставления регистров и использования JL
| BRNCH |   JNZ  | 8 bit OFFSET  | F9 1111 1001| | if (v != 0)
| BRNCH |    JC  | 8 bit OFFSET  | FA 1111 1010| | if (CF == 1)
| BRNCH |   JNC  | 8 bit OFFSET  | FB 1111 1011| | if (CF == 0)   // выкинуть инструкции, что реально не будут использоваться
| BRNCH |   JHC  | 8 bit OFFSET  | FC 1111 1100| | if (HCF == 1)  // вкрутить что-то вроде NJMP offset (хотя текущий JMP так и делает?)
| BRNCH |  JNHC  | 8 bit OFFSET  | FD 1111 1101| | if (HCF == 0)  // (и делать реальный переход на offset*16, к примеру)
| BRNCH | AFCALL | Hi 8 bit ADDR | FE 1111 1110| | Absolute Far Call - 8 bit ADDR кладём в старшие биты и зануляем младшие
| BRNCH |NOP/STOP|   1111 111x   | FF 1111 1111| | NOP - хорошо бы чтоб имел код 0xFF, это позволит думать, что не прошитая память заполнена NOP-ами
|----------------------------------------------|-| если младший бит 0, то это STOP (отладочный стоп), плюс 7 бит на полёт фантазии
           0     1    2    3    4     5    6    7
 names: | HCF | CF | ZF | LF | ~EQ | GF | BF | -- | read only
*/

#include <conio.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stack>
#include <vector>

#include "offset2int.h"
#include "read_hex.h"

using namespace std;

const char* OpNames[] {
  // 0       1      2     3      4      5      6      7
  "ADD", "ADDC", "AND", "OR", "XOR", "MUL", "UNO", "MOV",
  // 8     9     A      B     C      D       E        F
  "LPM", "LD", "IN", "OUT", "ST", "CMP", "CMPC", "BRNCH"
};

const char* BranchNames[] {
  //  0      1      2       3     4     5      6     7
  "CALL", "JMP", "RET", "RETI", "JL", "JE", "JNE", "JG",
  // 8     9     A      B      C       D         E      F
  "JZ", "JNZ", "JC", "JNC", "JHC", "JNHC", "AFCALL", "NOP"
};

const char* RegNames[] {
  "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"
};

const char* PtrNames[] {
  "X", "Y", "Z", "SP"
};

const char* UnoNames[] {
  "INV", "SWAP", "LSR", "LSRC"
};

const char* StopName {
  "STOP"
};

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

enum FLAGS {
  HCF = 0, CF = 1, ZF = 2, LF = 3, EF = 4, GF = 5, BF = 6, RSRV = 7, FLAGS_CNT = 8
};

const char* FlagNames[FLAGS_CNT] {
  "HCF", "CF", "ZF", "LF", "EF", "GF", "BF", "R"
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

class Cmd {
 public:
  Cmd(uint16_t cmd): cmd_(cmd) {}

  uint16_t cmd() { return cmd_; }
  uint8_t op() { return (cmd_ >> 8) & 0xF0; }

  virtual string Params() { return ""; }

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

  virtual string Params() {
    string rsrc = is_cnst() ? to_string(cmd_ & 0xFF) : RegNames[src()];
    return string(" ") + RegNames[dst()] + string(", ") + rsrc;
  }
};

//|0 1 2 3  4 5 6 7 8 9 A B C D E F|
//|   UNO |  DST |0|-|TYP|F|-|-|-|-| 60 0110 0000|*| унарные команды не используют операнд SRC, поэтому нельзя использовать инверторы и отдельные нибблы
class UnaryCmd: public Cmd {
 public:
  UnaryCmd(uint16_t cmd): Cmd(cmd) {}

  uint8_t dst() { return (cmd_ >> 9) & 0x07; }
  uint8_t type() { return (cmd_ >> 5) & 0x03; }

  virtual string Params() {
    string res = UnoNames[type()];
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

  virtual string Params() {
    string suffix;
    if (autodec())
      suffix += "D";
    if (autoinc())
      suffix += "I";
    if (offs()) {
      suffix += " + ";
      suffix += to_string(offs());  // offs may have value from -8 to +7 so I have to convert it to "int"
    }

    if (op() == 0x90)  // LD
      return string(" ") + RegNames[reg()] + string(", ") + PtrNames[ptr()] + suffix;
    else if (op() == 0xC0)  // ST
      return string(" ") + PtrNames[ptr()] + suffix + string(", ") + RegNames[reg()];
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

  virtual string Params() {
    if (op() == 0xA0)
      return string(" ") + RegNames[reg()] + string(", PINS") + to_string(port());
    else if (op() == 0xB0)
      return " PORT" + to_string(port()) + ", " + RegNames[reg()];
    else
      return "error";
  }
};

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
    uint8_t rdst = ActiveRegsBank()[acmd.dst()];
    uint8_t rsrc = acmd.is_cnst() ? acmd.cnst() : ActiveRegsBank()[acmd.src()];
    switch (op) {
      case 0x00: Flags[CF] = (rdst + rsrc > 255); rdst += rsrc; break;  // ADD // TODO: add sub operation here
      case 0x10: Flags[CF] = (rdst + rsrc > 255); rdst += rsrc + Flags[CF]; break;  // ADDC
      case 0x20: rdst &= rsrc; break;  // AND
      case 0x30: rdst |= rsrc; break;  // OR
      case 0x40: rdst ^= rsrc; break;  // XOR
      case 0x50: rdst *= rsrc; break;  // MUL  // TODO: have to write into register pair
      // 0x60 is UNO op
      case 0x70: rdst = rsrc; break;   // MOV
      case 0x80: break;  // LPM operation must be here
      default: cout << "Unknown op for this switch: " << op << endl; break;
    }
    if (op != 0x70)
      Flags[ZF] = (rdst == 0);
    ActiveRegsBank()[acmd.dst()] = rdst;
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
