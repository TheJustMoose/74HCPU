#include <doctest.h>

#include "assm.h"

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

TEST_CASE("check empty db consts class") {
  vector<uint8_t> data;
  vector<uint16_t> code;
  // check empty DBConsts
  DBConsts dbc(data);
  CHECK_EQ(dbc.GetSize(), 0);
  dbc.OutCode(code);
  CHECK_EQ(code.size(), 0);
  CHECK_EQ(dbc.Join().size(), 0);
}

TEST_CASE("check simple db consts class") {
  vector<uint8_t> data {1, 2, 3};
  vector<uint16_t> code;
  // check simple DBConsts
  DBConsts dbc(data);
  CHECK_EQ(dbc.GetSize(), 3);
  dbc.OutCode(code);
  CHECK_EQ(code.size(), 3);
  CHECK_EQ(code[0], 1);
  CHECK_EQ(code[1], 2);
  CHECK_EQ(code[2], 3);
  CHECK_EQ(dbc.Join(), "1|2|3");
}

TEST_CASE("check db consts class with offset") {
  vector<uint8_t> data {1, 2, 3};
  vector<uint16_t> code;
  // check simple DBConsts
  DBConsts dbc(data);
  dbc.SetAddress(30);
  CHECK_EQ(dbc.GetSize(), 3);
  dbc.OutCode(code);
  CHECK_EQ(code.size(), 33);
  CHECK_EQ(code[0], 0xFFFF);
  CHECK_EQ(code[30], 1);
  CHECK_EQ(code[31], 2);
  CHECK_EQ(code[32], 3);
  CHECK_EQ(dbc.Join(), "1|2|3");
}
