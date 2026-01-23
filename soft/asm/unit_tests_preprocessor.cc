#include <doctest.h>

#include <map>
#include <string>

#include "preprocessor.h"

using namespace std;

TEST_CASE("check .def detection & simple defines") {
  map<int, string> lines {
    {1, "add ACC, r1"},
    {2, "PUSH r0"},
    {3, "PUSH r7"},
    {10, ".def ACC R0"},
    {11, ".def PUSH(r) ST SPD, r"},
    {13, ";.org 1000h"}
  };

  Preprocessor pre;
  CHECK(!pre.Preprocess(nullptr));

  CHECK(pre.Preprocess(&lines));
  REQUIRE(lines.size() == 4);
  // now all string items separated by space
  CHECK(lines[1] == "add R0 , r1");
  CHECK(lines[2] == "ST SPD , r0");
  CHECK(lines[3] == "ST SPD , r7");
  CHECK(lines[4] == "");
}

TEST_CASE("check labels") {
  map<int, string> lines {
    {1, "label:PUSH r0"},
    {2, "lsr r0"},
    {11, ".def PUSH(r) ST SPD, r"}
  };

  Preprocessor pre;
  CHECK(!pre.Preprocess(nullptr));

  CHECK(pre.Preprocess(&lines));
  REQUIRE(lines.size() == 2);
  // now all string items separated by space
  CHECK(lines[1] == "label:ST SPD , r0");
  CHECK(lines[2] == "lsr r0");
}

TEST_CASE("check multilines") {
  Preprocessor pre;
  CHECK(!pre.Preprocess(nullptr));

  map<int, string> lines {
    {1, "One line \\"},
    {2, "or another...\\"}  // error
  };
  CHECK(!pre.Preprocess(&lines));  // and it have to detect it!

  map<int, string> good_lines {
    {1, "One line \\"},
    {2, "or another..."}
  };
  CHECK(pre.Preprocess(&good_lines));
}
