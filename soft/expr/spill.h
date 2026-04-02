#pragma once

#include <cstring>
#include <string>
#include <vector>

class ISpillable {
 public:
  virtual ~ISpillable() = default;
  virtual void Spill(size_t reg_idx, /*string some_var_name,*/ std::vector<std::string> &res) = 0;
};
