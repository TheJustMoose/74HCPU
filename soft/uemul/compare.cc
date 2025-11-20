#include "compare.h"

Relation Compare(uint8_t left, uint8_t right) {
  if (left < right) {
    return { true, false, false };
  } else if (left == right) {
    return { false, true, false };
  } else {
    return { false, false, true };
  }
}
