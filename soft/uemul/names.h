#pragma once

#include "flags.h"

inline const char* OpNames[] {
  // 0       1      2     3      4      5      6      7
  "ADD", "ADDC", "AND", "OR", "XOR", "MUL", "UNO", "MOV",
  // 8     9     A      B     C      D       E        F
  "LPM", "LD", "IN", "OUT", "ST", "CMP", "CMPC", "BRNCH"
};

inline const char* BranchNames[] {
  //  0      1      2       3     4     5      6     7
  "CALL", "JMP", "RET", "RETI", "JL", "JE", "JNE", "JG",
  // 8     9     A      B      C       D         E      F
  "JZ", "JNZ", "JC", "JNC", "JHC", "JNHC", "AFCALL", "NOP"
};

inline const char* RegNames[] {
  "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"
};

inline const char* PtrNames[] {
  "X", "Y", "Z", "SP"
};

inline const char* UnoNames[] {
  "INV", "SWAP", "LSR", "LSRC"
};

inline const char* StopName {
  "STOP"
};

inline const char* FlagNames[flags::CNT] {
  "HCF", "CF", "ZF", "LF", "EF", "GF", "BF", "R"
};
