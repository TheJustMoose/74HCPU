#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

class ISpillable {
 public:
  virtual ~ISpillable() = default;

  virtual void Spill(size_t reg_idx,     // will spill this register
                     uint16_t var_addr,  // to this addr
                     std::vector<std::string> &res) = 0;
};
