#pragma once

#include <cstdint>

enum PTR : uint16_t {
  // Pointer register pairs
  rX = 0, rY = 1, rSP = 2, rV = 3,
  // Pointer register mask
  rMask = 0x3,
  // Flags
  rInc = 0x10, rDec = 0x20,
  // Same pointers but with post increment
  rXI = rX | rInc, rYI = rY | rInc, rSPI = rSP | rInc, rVI = rV | rInc,
  // Same pointers but with post decrement
  rXD = rX | rDec, rYD = rY | rDec, rSPD = rSP | rDec, rVD = rV | rDec,
  rUnkPtr = 0x100
};
