#pragma once

#include <bitset>
#include <cstdint>
#include <optional>

const size_t ROM_SIZE = 65536;

class SlotAllocator {
 public:
  std::optional<uint16_t> GetFirstEmptyWindowWithSize(uint16_t size);

  bool IsOccupied(uint16_t addr) {
    // addr is uint16_t and has value from 0 to 65535
    // Thus, any addr is suitable for occupied_addresses_
    return occupied_addresses_[addr];
  }

  void OccupyIt(uint16_t addr) {
    occupied_addresses_[addr] = true;
  }

  bool OccupyIt(uint16_t addr, uint16_t size);

  void SetVerbose(bool verbose) {
    verbose_ = verbose;
  }

 private:
  std::bitset<ROM_SIZE> occupied_addresses_ {};
  bool verbose_ {false};
};
