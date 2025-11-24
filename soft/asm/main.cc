#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "assm.h"

using namespace std;

void help() {
  const char* help_lines[] = {
      "74HCPU assembler v 0.4\n",
      "Support next operations:\n",
      "arithmetic: ADD, ADDC, AND, OR, XOR, MUL, UNO (SWAP, INV, LSR, LSRC), MOV\n",
      "memory: LPM, LD, ST\n",
      "port: IN, OUT\n",
      "compare: CMP, CMPC\n",
      "jmp: CALL, JMP, RET, JZ, JL, JNE, JE, JG, JC, JNZ, JNC, JHC, JNHC, STOP, AFCALL, NOP\n",
      "Registers: R0, R1, R2, R3, R4, R5, R6, R7\n",
      "Register pointers: X(XL+XH), Y(YL+YH), Z(ZL+ZH), SP(SPL+SPH)\n",
      "Auto-increment pointers: XI, YI, ZI, SPI\n",
      "Auto-decrement pointers: XD, YD, ZD, SPD\n",
      "PORTS: PORT0-31, PIN0-31\n",
      "Macro: LO()/HI() for pointers, for example:\nmov YL, LO(StringName)\n",
      "Directives:\n.org 1000h\n.str S \"Some str\"\n.def FROM TO\n.def FROM(param) TO param, bla-bla\n",
      ".def push(r) ST SPD, r ; example for .def with param\n",
      "\nRun:\n",
      "74hc-asm.exe src.asm [-pre]\n",
      " -pre will print preprocessed src.asm\n",
      "74hc-asm.exe src.asm out.hex\n",
      " will create result binary out.hex\n",
      nullptr
  };

  int i = 0;
  while (help_lines[i]) {
    cout << help_lines[i];
    i++;
  }
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

  if (argc < 2) {
    help();
    return 0;
  }

  if (argc >= 2 && argv[1]) {
    string cmd(argv[1]);
    if (cmd == "help") {
      help();
      return 0;
    }

    bool show_pre {false};
    if (argc > 2 && string(argv[2]) == "-pre")
      show_pre = true;

    // okay, probably cmd is file name ;)
    try {
      Assembler assm;
      bool res = assm.process(cmd, show_pre);
      if (argc > 2 && argv[2] && !show_pre && !res)
        assm.write_binary(argv[2]);
      return res;
    } catch (const char* e) {
      cout << "Error: " << e << endl;
    } catch (std::exception e) {
      cout << "Error: " << e.what() << endl;
    }
  }

  cout << "argc: " << argc << endl;
  return 0;
}
