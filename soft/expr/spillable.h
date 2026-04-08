#pragma once

#include <cstdint>
#include <string>
#include <vector>

class ISpillable {
 public:
  virtual ~ISpillable() = default;

  virtual void Spill(size_t reg_idx,
                     std::vector<std::string> &res,
                     std::map<std::string, uint16_t>& var_addrs) = 0;
};
