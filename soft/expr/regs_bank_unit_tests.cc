#include <doctest.h>

#include "regs_bank.h"

using namespace std;

TEST_CASE("check RegsBank0::FindRegFor") {
  RegsBank0 rb;
  CHECK(rb[0].empty());
  CHECK(rb[7].empty());

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

TEST_CASE("check FindRegFor with many variables") {
  RegsBank0 rb;
  vector<string> res_code;
  string reg;

  for (int i = 0; i < 10; i++) {
    string si = to_string(i);
    reg = rb.FindRegFor("var" + si, res_code);
    if (i >= 8)
      CHECK_EQ(reg,  "R" + to_string(i % 8));  // use registers in a circle
    else
      CHECK_EQ(reg, "R" + si);
  }
}
