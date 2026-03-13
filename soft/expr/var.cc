#include "var.h"
#include "var_size.h"

uint8_t Var::size() const {
  return ::var_size(data_type, false);
}
