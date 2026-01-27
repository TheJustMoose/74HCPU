#pragma once

#include <cstdint>

enum REG : uint16_t {
  // Arithmetic registers
  rR0 = 0, rR1 = 1, rR2 = 2, rR3 = 3, rR4 = 4, rR5 = 5, rR6 = 6, rR7 = 7,   // Bank 0
  // Pointer registers
  rXL = 0, rXH = 1, rYL = 2, rYH = 3, rVL = 4, rVH = 5, rSPL = 6, rSPH = 7, // Bank 1
  rUnkReg = 0x100
};
