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

TEST_CASE("check 1 byte .db directive") {
  std::map<int, std::string> lines {
    {1, ".db ONE 10"}
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  REQUIRE_EQ(code.size(), 1);
  CHECK_EQ(code[0], 10);
}

TEST_CASE("check multi byte .db directive") {
  std::map<int, std::string> lines {
    {1, ".db MANY 1, 2, 3, 4, 5"}
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  REQUIRE_EQ(code.size(), 5);
  CHECK_EQ(code[0], 1);
  CHECK_EQ(code[1], 2);
  CHECK_EQ(code[2], 3);
  CHECK_EQ(code[3], 4);
  CHECK_EQ(code[4], 5);
}

TEST_CASE("check 1 byte .db directive + code") {
  std::map<int, std::string> lines {
    {1, "MOV R0, 1"},
    {2, ".db ONE 10"}
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  REQUIRE_EQ(code.size(), 2);
  CHECK_EQ(code[0], 0x7101);  // code
  CHECK_EQ(code[1], 10);  // data
}

TEST_CASE("check .str directive") {
  std::map<int, std::string> lines {
    {1, ".str S \"abc\""}
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  //           0000 + DBGI + addr of str + 'S' + 0
  size_t dbg_info_size = 8 + 1 + 1 + 1;

  REQUIRE_EQ(code.size(), 4 + dbg_info_size);  // abc + 0 + dbg_info_size
  CHECK_EQ(code[0], 'a');
  CHECK_EQ(code[1], 'b');
  CHECK_EQ(code[2], 'c');
  CHECK_EQ(code[3], 0);
}
