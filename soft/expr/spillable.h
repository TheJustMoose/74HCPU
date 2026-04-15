#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

class ISpillable {
 public:
  virtual ~ISpillable() = default;

  virtual void Spill(size_t reg_idx,            // will spill this register
                     uint16_t var_addr,         // to this addr
                     std::string var_name) = 0;

  virtual void Fill(std::string var_name,       // will read this variable
                    std::string reg_name) = 0;  // to this register
};
