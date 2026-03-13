#include "var.h"
#include "var_size.h"

int Var::size() const {
  return ::var_size(data_type, false);
}
