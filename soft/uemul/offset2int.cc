#include "offset2int.h"

#include <iostream>

using namespace std;

int16_t OffsetToInt(uint8_t offset) {
  if (offset > 15) {  // we need only 4 low bits
    cout << "You can use only 4 low bits" << endl;
    return 0;
  }
  uint16_t res = offset;
  if (offset & 0x08)  // offset is 4 bit two's complement value
    res |= 0xFFF0;    // so bit 3 is the sign bit and we have to extend it to high bits
  return res;  // now it's int!
}

int16_t ByteOffsetToInt(uint8_t offset) {
  uint16_t res = offset;
  if (offset & 0x80)  // offset is 8 bit two's complement value
    res |= 0xFF00;    // so bit 7 is the sign bit and we have to extend it to high bits
  return res;  // now it's int!
}
