#include <stdio.h>
#include <cstdint>
#include <iostream>
#include <vector>

#include "cpu.h"
#include "names.h"
#include "read_hex.h"

using namespace std;

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

  CPU cpu(Cmds);
  cpu.Run(dbg);

  return 0;
}
