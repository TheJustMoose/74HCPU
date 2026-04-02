#include <doctest.h>
#include <trompeloeil.hpp>
#include <doctest/trompeloeil.hpp>  // doctest adapter

using trompeloeil::_;

#include "regs_bank.h"
#include "spillable.h"

using namespace std;

TEST_CASE("check RegsBank0::FindRegFor") {
  RegsBank0 rb {nullptr};
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

TEST_CASE("check RegsBank0::FindRegFor with many variables") {
  RegsBank0 rb {nullptr};
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

class MockSpillable : public ISpillable {
 public:
  // this macros will create mock method:
  MAKE_MOCK2(Spill, void(size_t reg_idx, vector<string> &res), override);
};

TEST_CASE("check RegsBank0::Spill") {
  MockSpillable mockSpill;
  RegsBank0 rb {&mockSpill};

  // mockSpill::Spill should be called with any args (trompeloeil::_) two times
  REQUIRE_CALL(mockSpill, Spill(trompeloeil::_, trompeloeil::_)).TIMES(2);

  vector<string> res_code;
  for (int i = 0; i < 10; i++) {
    string reg = rb.FindRegFor("var" + to_string(i), res_code);
  }
}
