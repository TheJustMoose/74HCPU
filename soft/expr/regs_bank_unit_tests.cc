#include <doctest.h>

#include "regs_bank.h"

using namespace std;

TEST_CASE("check C++") {
  RegsBank0 rb;
  CHECK(rb[0].empty());

  vector<string> res_code;

  string reg = rb.FindRegFor("var", res_code);
  CHECK_EQ(reg, "R0");

  reg = rb.FindRegFor("abc", res_code);
  CHECK_EQ(reg, "R1");

  reg = rb.FindRegFor("def", res_code);
  CHECK_EQ(reg, "R2");

  reg = rb.FindRegFor("i", res_code);
  CHECK_EQ(reg, "R3");

  reg = rb.FindRegFor("i", res_code);
  CHECK_EQ(reg, "R3");  // have to get same register for same variable
}
