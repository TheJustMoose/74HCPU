#include <doctest.h>
#include <trompeloeil.hpp>
#include <doctest/trompeloeil.hpp>  // doctest adapter

#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using trompeloeil::_;

#include "regs_bank.h"
#include "spillable.h"

using namespace std;

TEST_CASE("check RegsBank0::FindRegFor") {
  map<string, uint16_t> var_addrs;
  RegsBank0 rb {nullptr, var_addrs};
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
  map<string, uint16_t> var_addrs;
  RegsBank0 rb {nullptr, var_addrs};
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
  MAKE_MOCK3(Spill, void(size_t reg_idx, uint16_t var_addr,
                         vector<string> &res),
                    override);
};

TEST_CASE("check RegsBank0::Spill") {
  MockSpillable mockSpill;
  map<string, uint16_t> var_addrs {
    {"var0", 10},  // R0
    {"var1", 20},  // R1
    {"var2", 30},  // R2
    {"var3", 40},  // R3
    {"var4", 50},  // R4
    {"var5", 60},  // R5
    {"var6", 70},  // R6
    {"var7", 80},  // R7
    {"var8", 90},  // R8
    {"var9", 100}  // R9
  };

  cout << endl << "check RegsBank0::Spill" << endl;

  RegsBank0 rb {&mockSpill, var_addrs};

  // mockSpill::Spill should be called two times
  // first call: var8 will stored in R0, R0/var0 have to be spilled into RAM
  REQUIRE_CALL(mockSpill, Spill( 0, 10, trompeloeil::_ )).TIMES(1);
  // second call: var9 will stored in R1, R1/var1 have to be spilled into RAM
  REQUIRE_CALL(mockSpill, Spill( 1, 20, trompeloeil::_ )).TIMES(1);

  vector<string> res_code;
  for (int i = 0; i < 10; i++) {
    rb.FindRegFor("var" + to_string(i), res_code);
  }

  cout << "check RegsBank0::Spill finished" << endl;
}
