#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "offset2int.h"

using namespace std;

TEST_CASE("test OffsetToInt") {
  // 4 bit two's complement value
  CHECK(OffsetToInt(0) == 0);
  CHECK(OffsetToInt(1) == 1);
  CHECK(OffsetToInt(7) == 7);
  CHECK(OffsetToInt(8) == -8);
  CHECK(OffsetToInt(15) == -1);
}

TEST_CASE("test ByteOffsetToInt") {
  // 8 bit two's complement value
  CHECK(ByteOffsetToInt(0) == 0);
  CHECK(ByteOffsetToInt(1) == 1);
  CHECK(ByteOffsetToInt(127) == 127);
  CHECK(ByteOffsetToInt(128) == -128);
}
