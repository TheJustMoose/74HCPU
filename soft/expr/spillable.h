#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

class ISpillable {
 public:
  virtual ~ISpillable() = default;

  virtual void Spill(size_t reg_idx,        // will spill this register
                     std::string var_name,  // with this name
                     std::vector<std::string> &res,
                     std::map<std::string, uint16_t>& var_addrs) = 0;
};
