#pragma once

#include <cstdint>

class Relation {
 public:
  bool less {false};
  bool equal {true};
  bool greater {false};
};

Relation Compare(uint8_t left, uint8_t right);
