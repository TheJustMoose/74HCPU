#pragma once

#include <bitset>
#include <cstdint>
#include <optional>

class SlotAllocator {
 public:
  std::optional<uint16_t> Allocate(uint16_t size);

  bool IsOccupied(uint16_t addr) {
    return occupied_addresses_[addr];
  }

  void OccupyIt(uint16_t addr) {
    occupied_addresses_[addr] = true;
  }

  void SetVerbose(bool verbose) {
    verbose_ = verbose;
  }

 protected:
  std::optional<uint16_t> GetFirstEmptyWindowWithSize(uint16_t size);

 private:
  std::bitset<65536> occupied_addresses_ {};
  bool verbose_ {false};
};
