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
  vector<string> res_code;
  RegsBank0 rb {nullptr, var_addrs, res_code};
  CHECK(rb[0].empty());
  CHECK(rb[7].empty());


  string reg = rb.FindRegFor("var");
  CHECK_EQ(reg, "R0");

  reg = rb.FindRegFor("abc");
  CHECK_EQ(reg, "R1");

  reg = rb.FindRegFor("def");
  CHECK_EQ(reg, "R2");

  reg = rb.FindRegFor("i");
  CHECK_EQ(reg, "R3");

  reg = rb.FindRegFor("i");
  CHECK_EQ(reg, "R3");  // have to get same register for same variable
}

TEST_CASE("check RegsBank0::FindRegFor with many variables") {
  map<string, uint16_t> var_addrs;
  vector<string> res_code;
  RegsBank0 rb {nullptr, var_addrs, res_code};
  string reg;

  for (int i = 0; i < 10; i++) {
    string si = to_string(i);
    reg = rb.FindRegFor("var" + si);
    if (i >= 8)
      CHECK_EQ(reg,  "R" + to_string(i % 8));  // use registers in a circle
    else
      CHECK_EQ(reg, "R" + si);
  }
}

class MockSpillable : public ISpillable {
 public:
  // this macros will create mock method:
  MAKE_MOCK4(Spill, void(size_t reg_idx, uint16_t var_addr,
                         vector<string> &res, string var_name),
                    override);

  MAKE_MOCK1(Fill, void (string var_name),
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

  vector<string> res_code;
  RegsBank0 rb {&mockSpill, var_addrs, res_code};

  // mockSpill::Spill should be called two times
  // first call: var8 will stored in R0, R0/var0 have to be spilled into RAM
  REQUIRE_CALL(mockSpill, Spill( 0, 10, trompeloeil::_, "var0" )).TIMES(1);
  // second call: var9 will stored in R1, R1/var1 have to be spilled into RAM
  REQUIRE_CALL(mockSpill, Spill( 1, 20, trompeloeil::_, "var1" )).TIMES(1);

  for (int i = 0; i < 10; i++) {
    rb.FindRegFor("var" + to_string(i));
  }
}

TEST_CASE("check RegsBank0::DumpRegs") {
  map<string, uint16_t> var_addrs;
  vector<string> res;
  RegsBank0 rb {nullptr, var_addrs, res};

  rb.FindRegFor("abc");
  rb.FindRegFor("i");
  rb.FindRegFor("j");
  rb.FindRegFor("str");
  rb.FindRegFor("array");
  rb.FindRegFor("t0");
  rb.FindRegFor("t1");
  rb.FindRegFor("__x");

  CHECK_EQ(rb[0], "abc");  // also check operator[]
  CHECK_EQ(rb[7], "__x");

  CHECK_EQ(rb.DumpRegs(), "// R0: abc, R1: i, R2: j, R3: str, R4: array, R5: t0, R6: t1, R7: __x");
}

TEST_CASE("check RegsBank0::FreeTheRegister") {
  map<string, uint16_t> var_addrs;
  vector<string> res;
  RegsBank0 rb {nullptr, var_addrs, res};

  rb.FindRegFor("abc");
  CHECK_EQ(rb[0], "abc");

  rb.FreeTheRegister(0);
  CHECK_EQ(rb[0], "");
}

TEST_CASE("check RegsBank0::GetIndexOfVar") {
  map<string, uint16_t> var_addrs;
  vector<string> res;
  RegsBank0 rb {nullptr, var_addrs, res};

  rb.FindRegFor("x");  // 0
  rb.FindRegFor("y");  // 1
  rb.FindRegFor("z");  // 2

  CHECK_EQ(rb.GetIndexOfVar("y"), 1);
}
