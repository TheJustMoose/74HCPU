#include "var_size.h"

uint8_t var_size(DataType dt, bool is_pointer) {
  if (is_pointer)
    return 2;
  if (dt == dtInt)
    return 2;
  if (dt == dtByte)
    return 1;
  return 0;
}
