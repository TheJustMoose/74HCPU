#include <doctest.h>

#include <string>
#include <vector>

#include "str_util.h"

using namespace std;

TEST_CASE("check ToUpper") {
  CHECK_EQ(ToUpper(""), "");
  CHECK_EQ(ToUpper("AAA"), "AAA");
  CHECK_EQ(ToUpper("aaa"), "AAA");
  CHECK_EQ(ToUpper("aAa"), "AAA");
}

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

  vector<string> c6 = Split(".db N 100, 200, 300");
  REQUIRE(c6.size() == 7);
  CHECK(c6[0] == ".db");
  CHECK(c6[1] == "N");
  CHECK(c6[2] == "100");
  CHECK(c6[3] == ",");
  CHECK(c6[4] == "200");
  CHECK(c6[5] == ",");
  CHECK(c6[6] == "300");
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

TEST_CASE("check NormalizeLine") {
  CHECK_EQ(NormalizeLine("  ADD   R0,   R1"), "ADD R0, R1");
  // Do not change string literals!
  CHECK(NormalizeLine("\"  ADD   R0,   R1\"") == "\"  ADD   R0,   R1\"");
  CHECK_EQ(NormalizeLine("\"1\""), "\"1\"");
  CHECK_EQ(NormalizeLine("   \"a  b  c\"   "),
                            "\"a  b  c\"");
  // Remove comments
  CHECK_EQ(NormalizeLine("ADD R1, R2   ; cmt"), "ADD R1, R2");
  // Do not remove comments if it is in string literal
  CHECK_EQ(NormalizeLine(".str X \"str with ; which is not comment\""),
                         ".str X \"str with ; which is not comment\"");
  CHECK_EQ(NormalizeLine(".str X \"ab;cd\""), ".str X \"ab;cd\"");

  // check empty lines
  CHECK(NormalizeLine("") == "");
  CHECK(NormalizeLine(" ") == "");
  CHECK(NormalizeLine("  ") == "");
  CHECK(NormalizeLine("\n") == "");

  // check many spaces in many places
  CHECK(NormalizeLine("a") == "a");
  CHECK(NormalizeLine(" a") == "a");
  CHECK(NormalizeLine("a ") == "a");
  CHECK(NormalizeLine(" a ") == "a");
  CHECK(NormalizeLine("  a  ") == "a");

  CHECK(NormalizeLine("a b") == "a b");
  CHECK(NormalizeLine("a  b") == "a b");
  CHECK(NormalizeLine("a   b") == "a b");
  CHECK(NormalizeLine(" a b") == "a b");
  CHECK(NormalizeLine("a b ") == "a b");
  CHECK(NormalizeLine("  a b  ") == "a b");

  // check comment removing
  CHECK(NormalizeLine("LSR R0 ; comment ") == "LSR R0");
  CHECK(NormalizeLine("LSR R0;comment ") == "LSR R0");
  CHECK(NormalizeLine("; comment ") == "");
  CHECK(NormalizeLine(";") == "");
  CHECK(NormalizeLine("\n;") == "");

  // check labels
  CHECK(NormalizeLine("label: mov  ax, bx  ; test cmd") == "label: mov ax, bx");
  CHECK(NormalizeLine("  LSR   R0  ") == "LSR R0");

  // check directives
  CHECK(NormalizeLine(".org 100h") == ".org 100h");
  CHECK(NormalizeLine("  .org 100h ; comment") == ".org 100h");

  // check different spaces
  CHECK(NormalizeLine("\t\tLSR R0  \n") == "LSR R0");
  CHECK(NormalizeLine("LSR\tR0") == "LSR\tR0");

  CHECK(NormalizeLine(";.org 1000h") == "");
}

TEST_CASE("check str_util::RemoveQuotes") {
  CHECK_EQ(RemoveQuotes("\"abc\""), "abc");
  CHECK_EQ(RemoveQuotes("abc"), "abc");
  CHECK_EQ(RemoveQuotes("x"), "x");
  CHECK_EQ(RemoveQuotes(""), "");
}

TEST_CASE("check str_util::StrToInt") {
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

  CHECK(!StrToInt("''", &val));
  CHECK(!StrToInt("'ab'", &val));

  CHECK(StrToInt("'0'", &val));
  CHECK('0' == val);
  CHECK(StrToInt("'A'", &val));
  CHECK('A' == val);
  CHECK(StrToInt("'a'", &val));
  CHECK('a' == val);
  CHECK(StrToInt("'?'", &val));
  CHECK('?' == val);
}

TEST_CASE("check Join") {
  CHECK(Join({}) == "");
  CHECK(Join({"1"}) == "1");
  CHECK(Join({"1", "2"}) == "1|2");
  CHECK(Join({"1", "2"}, ',') == "1,2");

  CHECK(JoinInt({}) == "");
  CHECK(JoinInt({1}) == "1");
  CHECK(JoinInt({1, 2}) == "1|2");
  CHECK(JoinInt({1, 2}, ',') == "1,2");
}

TEST_CASE("check Join") {
  CHECK_EQ(ToHexString(0), "0");
  CHECK_EQ(ToHexString(1), "1");
  CHECK_EQ(ToHexString(10), "A");
  CHECK_EQ(ToHexString(15), "F");
  CHECK_EQ(ToHexString(16), "10");
  CHECK_EQ(ToHexString(100), "64");
}
