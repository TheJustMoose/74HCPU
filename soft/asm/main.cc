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
      "74HCPU assembler v 0.5\n",
      "Support next operations:\n",
      "arithmetic: ADD, ADDC, AND, OR, XOR, MUL, UNO (SWAP, INV, LSR, LSRC), MOV\n",
      "memory: LPM, LD, ST\n",
      "video memory: LDV, STV\n",
      "port: IN, OUT, TOGL\n",
      "compare: CMP, CMPC\n",
      "jmp: CALL, JMP, RET, JZ, JL, JNE, JE, JG, JC, JNZ, JNC, JHC, JNHC, STOP, AFCALL, NOP\n",
      "Registers: R0, R1, R2, R3, R4, R5, R6, R7\n",
      "Register pointers: X(XL+XH), Y(YL+YH), V(VL+VH), SP(SPL+SPH)\n",
      "Auto-increment pointers: Xi, Yi, Vi, SPi\n",
      "Auto-decrement pointers: Xd, Yd, Vd, SPd\n",
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
      "74hc-asm.exe src.asm out.hex\n",
      " will create result binary out.hex\n",
      "74hc-asm.exe src.asm -v\n",
      " -v print more verbose information about src.asm\n",
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

  // fill vector
  vector<string> args;
  for (int i = 0; i < argc; i++)
    if (argv[i] != nullptr)
      args.push_back(argv[i]);

  bool show_pre {false};
  bool verbose {false};

  vector<size_t> to_delete;
  // check keys
  // args[0] is always process name
  for (size_t i = 1; i < args.size(); i++) {
    string arg = args[i];
    if (arg == "help") {
      help();
      return 0;
    }

    if (arg == "-pre") {
      show_pre = true;
      to_delete.push_back(i);
    }
    if (arg == "-v") {
      verbose = true;
      cout << "verbose - on" << endl;
      to_delete.push_back(i);
    }
  }

  // remove keys
  for (int i = to_delete.size() - 1; i >= 0; i--) {
    cout << "try to remove: " << i << ", with value: " << *(args.begin() + i) << endl;
    if (verbose)
      cout << "remove " << *(args.begin() + i) << endl;
    args.erase(args.begin() + i);  // will invalidate all iterators after current
  }

  if (verbose)
    cout << "remaining args: " << args.size() << endl;

  if (args.size() > 3 || args.size() == 1) {
    cout << "Error! asm require only 3 args: asm_name [asm_file] [hex_file] (and -v, -pre, help in any place)" << endl;
    return 1;
  }

  if (args.size() >= 2) {
    try {
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
  }

  return 0;
}
