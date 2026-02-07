#include <doctest.h>

#include "assm.h"

using namespace std;

class AsmWrapper: public Assembler {
 public:
  AsmWrapper() = default;
  void OutCodeWrapper(vector<uint16_t>& code) {
    OutCode(code);
  }

  uint16_t GetMaxCodeAddressWrapper(bool* occupied) {
    return GetMaxCodeAddress(occupied);
  }
};

TEST_CASE("check assembler class") {
  std::map<int, std::string> lines {
    {1, "MOV R0, 1"},
    {2, "MOV R1, R0"},
    {3, "STOP"},
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  REQUIRE_EQ(code.size(), 3);
  CHECK_EQ(code[0], 0x7101);
  CHECK_EQ(code[1], 0x7200);
  CHECK_EQ(code[2], 0xFFFE);
  bool occupied {false};
  CHECK_EQ(asmw.GetMaxCodeAddressWrapper(&occupied), 2);
}
/*
TEST_CASE("check .db directive") {
  std::map<int, std::string> lines {
    {1, ".db ONE 10"}
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  CHECK_EQ(code.size(), 1);
  CHECK_EQ(code[0], 10);
}
*/
