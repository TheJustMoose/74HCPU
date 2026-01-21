#include <doctest.h>

#include <string>
#include <vector>

#include "str_util.h"

using namespace std;

TEST_CASE("check IsSpace") {
  CHECK(IsSpace(' '));
  CHECK(IsSpace('\t'));
  CHECK(IsSpace('\n'));
  CHECK(IsSpace('\r'));
  CHECK(!IsSpace('A'));
}

TEST_CASE("check split") {
  CHECK(Split("").size() == 0);

  CHECK(Split("test").size() == 1);

  vector<string> v = Split("test test");
  REQUIRE(v.size() == 2);
  CHECK(v[0] == "test");
  CHECK(v[1] == "test");

  vector<string> c1 = Split("add r0, 10");
  REQUIRE(c1.size() == 4);
  CHECK(c1[0] == "add");
  CHECK(c1[1] == "r0");
  CHECK(c1[2] == ",");
  CHECK(c1[3] == "10");

  vector<string> c2 = Split(" add  r0 , 10 ");
  REQUIRE(c2.size() == 4);
  CHECK(c2[0] == "add");
  CHECK(c2[1] == "r0");
  CHECK(c2[2] == ",");
  CHECK(c2[3] == "10");

  vector<string> c3 = Split(".org 100h");
  REQUIRE(c3.size() == 2);
  CHECK(c3[0] == ".org");
  CHECK(c3[1] == "100h");

  vector<string> c4 = Split(".def\tACC\tR0\n");
  REQUIRE(c4.size() == 3);
  CHECK(c4[0] == ".def");
  CHECK(c4[1] == "ACC");
  CHECK(c4[2] == "R0");

  vector<string> c5 = Split(".db N 100");
  REQUIRE(c5.size() == 3);
  CHECK(c5[0] == ".db");
  CHECK(c5[1] == "N");
  CHECK(c5[2] == "100");
}

TEST_CASE("check split command parts") {
  vector<string> t1 = SplitToCmdParts("MOV a, b");
  CHECK(t1.size() == 3);
  CHECK(t1[0] == "MOV");
  CHECK(t1[1] == "A");  // Args converted to upper during split
  CHECK(t1[2] == "B");

  vector<string> t2 = SplitToCmdParts("ST X+1, R0");
  CHECK(t2.size() == 3);
  CHECK(t2[0] == "ST");
  CHECK(t2[1] == "X+1");
  CHECK(t2[2] == "R0");

  vector<string> t3 = SplitToCmdParts("LD R1, Y-1");
  CHECK(t3.size() == 3);
  CHECK(t3[0] == "LD");
  CHECK(t3[1] == "R1");
  CHECK(t3[2] == "Y-1");

  vector<string> t4 = SplitToCmdParts("STOP");
  CHECK(t4.size() == 3);
  CHECK(t4[0] == "STOP");
  CHECK(t4[1] == "");
  CHECK(t4[2] == "");

  vector<string> t5 = SplitToCmdParts("MOV R0, L(R1)");
  CHECK(t5.size() == 3);
  CHECK(t5[0] == "MOV");
  CHECK(t5[1] == "R0");
  CHECK(t5[2] == "L(R1)");

  vector<string> t6 = SplitToCmdParts("MOV R0, `'L(R1) + 1");
  CHECK(t6.size() == 3);
  CHECK(t6[0] == "MOV");
  CHECK(t6[1] == "R0");
  CHECK(t6[2] == "`'L(R1)+1");
}

TEST_CASE("check str_util") {
  CHECK(NormalizeLine("  ADD   R0,   R1") == "ADD R0, R1");
  CHECK(NormalizeLine("\"  ADD   R0,   R1\"") == "\"  ADD   R0,   R1\"");

  int val;
  CHECK(!StrToInt("", &val));
  CHECK(!StrToInt("test", &val));

  CHECK(StrToInt("1", &val));
  CHECK(1 == val);

  CHECK(StrToInt("10", &val));
  CHECK(10 == val);

  CHECK(StrToInt("999999", &val));
  CHECK(999999 == val);

  CHECK(StrToInt("10H", &val));
  CHECK(16 == val);

  CHECK(StrToInt("0X10", &val));
  CHECK(16 == val);

  CHECK(StrToInt("0X1FFF", &val));
  CHECK(0x1FFF == val);

  CHECK(StrToInt("0O177", &val));
  CHECK(127 == val);

  CHECK(StrToInt("0B1010", &val));
  CHECK(10 == val);

  CHECK(StrToInt("0B0000", &val));
  CHECK(0 == val);

  CHECK(StrToInt("0B1111", &val));
  CHECK(15 == val);
}

TEST_CASE("check Join") {
  CHECK(Join({}) == "");
  CHECK(Join({"1"}) == "1");
  CHECK(Join({"1", "2"}) == "1|2");
  CHECK(Join({"1", "2"}, ',') == "1,2");
}
