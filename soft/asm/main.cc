#include <cstdint>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "assm.h"

using namespace std;

void help() {
  const char* help_lines[] = {
      "74HCPU assembler v 0.6\n",
      "Support next operations:\n",
      "arithmetic: ADD, ADDC, AND, OR, XOR, MUL, UNO (SWAP, INV, LSR, LSRC), MOV\n",
      "memory: LPM, LD, ST\n",
      "video memory: LDV, STV\n",
      "port: IN, OUT, TOGL\n",
      "compare: CMP, CMPC\n",
      "jmp: CALL, JMP, RET, JZ, JL, JNE, JE, JG, JC, JNZ, JNC, JHC, JNHC, STOP, AFCALL, NOP\n",
      "Registers: R0, R1, R2, R3, R4, R5, R6, R7\n",
      "Register pointers: X(XL+XH), Y(YL+YH), SP(SPL+SPH), V(VL+VH)\n",
      "Auto-increment pointers: Xi, Yi, SPi, Vi\n",
      "Auto-decrement pointers: Xd, Yd, SPd, Vd\n",
      "PORTS: PORT0-31, PIN0-31\n",
      "Macro: LO()/HI() for pointers, for example:\n mov YL, LO(StringName)\n",
      "Macro: L()/H()/~/'/` for registers, for example:\n",
      " ` - invert high nibble, ' - invert low nibble, ~ - invert whole byte\n",
      " L() - use low nibble, H() - use high nibble\n",
      " MOV R0, L(R1) - copy low nibble of R1 into R0\n",
      " MOV R0, `R1 - invert high nibble of R1 and copy both nibbles into R0\n",
      "Directives:\n.org 1000h\n.str S \"Some str\"\n.def FROM TO\n.def FROM(param) TO param, bla-bla\n",
      ".def push(r) ST SPD, r ; example for .def with param\n",
      ".db NAME 123 ; one const in ROM\n",
      ".db ARRAY 1, 2, 3 ; three consts in ROM\n",
      ".db MULTILINE \\ ; const on many lines of source code\n",
      "  1, 2, 3, \\\n",
      "  4, 5, 6\n",
      ".dw PTR NAME ; address of .db NAME 123 (see above)\n",
      "; <- this is comment",
      "\nRun:\n",
      "74hc-asm.exe src.asm [-pre]\n",
      " -pre will print preprocessed src.asm\n",
      "74hc-asm.exe src.asm [-v]\n",
      " -v print more verbose information about src.asm\n",
      "74hc-asm.exe src.asm [out.hex]\n",
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

  bool show_pre {false};
  bool verbose {false};

  // fill vector
  vector<string> args;
  for (int i = 0; i < argc; i++) {
    if (argv[i] == nullptr)
      continue;

    string arg = argv[i];
    if (arg.size() && arg[0] != '-')
      args.push_back(arg);
    if (arg == "-pre")
      show_pre = true;
    if (arg == "-v")
      verbose = true;
  }

  if (args.size() < 2 || args[1] == "help") {
    help();
    return 0;
  }

  if (verbose) {
    cout << "verbose - on" << endl;
    cout << "remaining args: " << args.size() << endl;
  }

  if (args.size() > 3) {
    cout << "Error! asm require only 3 args: asm_name [asm_file] [hex_file] (and -v, -pre in any place)" << endl;
    return 1;
  }

  try {  // Now args.size() may be equal 2 or 3
    Assembler assm;
    bool res = assm.Process(args[1], show_pre, verbose);
    if (args.size() > 2 && !show_pre && !res)
      assm.WriteBinary(args[2]);
    return res;
  } catch (const char* e) {
    cout << "Error: " << e << endl;
  } catch (std::exception e) {
    cout << "Error: " << e.what() << endl;
  }

  return 0;
}
