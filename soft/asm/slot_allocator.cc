#include "slot_allocator.h"

#include <iostream>

using namespace std;

optional<uint16_t> SlotAllocator::GetFirstEmptyWindowWithSize(uint16_t size) {
  if (verbose_)
    cout << "GetFirstEmptyWindowWithSize for size: " << hex << size << "h" << endl;

  uint16_t cnt {0};
  optional<uint16_t> beg;
  uint32_t end {1 << 16};
  for (uint32_t addr = 0; addr < end; addr++) {
    if (occupied_addresses_[addr]) {
      //cout << hex << setw(2) << addr << "h is occupied" << endl;
      cnt = 0;
      beg = nullopt;  // mark addr as empty
    } else {
      //cout << hex << setw(2) << addr << "h is free" << endl;
      cnt++;
      if (!beg.has_value()) {
        beg = addr;  // okay, first empty place was found
        if (verbose_)
          cout << hex << "now beg == " << addr << "h" << endl;
      }
      if (cnt >= size) {
        if (verbose_)
          cout << "Okay, will return address == " << hex << beg.value() << "h" << endl;
        return beg.value();
      }
    }
  }

  if (verbose_)
    cout << "Sufficient free space not found." << endl;

  return nullopt;
}

optional<uint16_t> SlotAllocator::Allocate(uint16_t size) {
  optional<uint16_t> addr = GetFirstEmptyWindowWithSize(size);
  if (!addr.has_value())
    return nullopt;

  for (uint16_t i = 0; i < size; i++)
    occupied_addresses_[addr.value() + i] = true;

  return addr;
}
