#include <doctest.h>
#include <iomanip>
#include <iostream>

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

  bool IsOccupiedWrapper(uint16_t addr) {
    return IsOccupied(addr);
  }

  uint16_t GetFirstEmptyWindowWithSizeWrapper(uint16_t size) {
    return GetFirstEmptyWindowWithSize(size);
  }

  uint16_t GetTotalSizeOfStringConstsWrapper() {
    return GetTotalSizeOfStringConsts();
  }

  uint16_t GetTotalSizeOfDBConstsWrapper() {
    return GetTotalSizeOfDBConsts();
  }

  uint16_t GetTotalSizeOfDWConstsWrapper() {
    return GetTotalSizeOfDWConsts();
  }

  void PrintAll() {
    PrintCode();
    PrintLabels();
    PrintOrgs();
    PrintDBs();
    PrintDWs();
  }
};

TEST_CASE("check assembler class") {
  map<int, string> lines {
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
  CHECK(occupied);
}

TEST_CASE("check 1 byte .db directive") {
  map<int, string> lines {
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
  map<int, string> lines {
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
  map<int, string> lines {
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
  map<int, string> lines {
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

TEST_CASE("check .dw directive") {
  map<int, string> lines {
    {1, ".db CNSTS 1, 2, 3, 4"},
    {2, ".dw PTR CNSTS"}
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  REQUIRE_EQ(code.size(), 5);
  CHECK_EQ(code[0], 1); // .db
  CHECK_EQ(code[1], 2);
  CHECK_EQ(code[2], 3);
  CHECK_EQ(code[3], 4);
  CHECK_EQ(code[4], 0); // .dw == address of .db
}

TEST_CASE("check all") {
  map<int, string> lines {
    {1, "MOV R0, 1"},            // 1
    {2, "MOV R1, R0"},           // 1
    {3, ".str S \"abc\""},       // 3 + 1
    {4, ".db CNSTS 1, 2, 3, 4"}, // 4
    {5, ".dw PTR1 CNSTS"},       // 1
    {6, ".dw PTR2 S"},           // 1
  };                             // == 12

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  //           0000 + DBGI + addr of str + 'S' + 0
  size_t dbg_info_size = 8 + 1 + 1 + 1;

  REQUIRE_EQ(code.size(), 12 + dbg_info_size);  // code size + dbg_info_size
  // code
  CHECK_EQ(code[0], 0x7101);
  CHECK_EQ(code[1], 0x7200);
  // data
  CHECK_EQ(code[2], 'a');
  CHECK_EQ(code[3], 'b');
  CHECK_EQ(code[4], 'c');
  CHECK_EQ(code[5], 0);   // ASCII Z
  CHECK_EQ(code[6], 1);   // 1
  CHECK_EQ(code[7], 2);   // 2
  CHECK_EQ(code[8], 3);   // 3
  CHECK_EQ(code[9], 4);   // 4
  CHECK_EQ(code[10], 6);  // PTR1
  CHECK_EQ(code[11], 2);  // PTR2 == string S address
  // dbg info prefix
  CHECK_EQ(code[12], 0);
  CHECK_EQ(code[13], 0);
  CHECK_EQ(code[14], 0);
  CHECK_EQ(code[15], 0);
  CHECK_EQ(code[16], 'D');
  CHECK_EQ(code[17], 'B');
  CHECK_EQ(code[18], 'G');
  CHECK_EQ(code[19], 'I');
  // dbg info
  CHECK_EQ(code[20], 2);
  CHECK_EQ(code[21], 'S');
  CHECK_EQ(code[22], 0);
}

TEST_CASE("check .org directive") {
  map<int, string> lines {
    {1, "MOV R0, 1"},
    {2, "MOV R1, R0"},
    {3, ".org 50h"},
    {4, "STOP"},
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  vector<uint16_t> code;
  asmw.OutCodeWrapper(code);

  REQUIRE_EQ(code.size(), 0x51);  // length == 0x51
  CHECK_EQ(code[0], 0x7101);      // min addr == 0
  CHECK_EQ(code[1], 0x7200);
  CHECK_EQ(code[0x50], 0xFFFE);   // max addr == 0x50

  // .org 50h means that STOP cmd will be placed at address 50h
  // so program length will be 51h
  bool occupied {false};
  CHECK_EQ(asmw.GetMaxCodeAddressWrapper(&occupied), 0x50);  // address, not length!
  CHECK(occupied);  // yes, I have STOP cmd at address 50h
}

TEST_CASE("check occupied_addresses_ bitset (empty)") {
  map<int, string> lines {
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  uint16_t first = asmw.GetFirstEmptyWindowWithSizeWrapper(1);
  CHECK_EQ(first, 0);
}

TEST_CASE("check occupied_addresses_ bitset (one block at addr 0)") {
  map<int, string> lines {
    {1, ".db BLOCK 1, 2, 3, 4"},
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  uint16_t first = asmw.GetFirstEmptyWindowWithSizeWrapper(1);
  CHECK_EQ(first, 4);
}

TEST_CASE("check occupied_addresses_ bitset (two blocks near)") {
  map<int, string> lines {
    {1, "STOP"},
    {2, ".org 20"},
    {3, "STOP"},
  };

  AsmWrapper asmw;
  asmw.Process(lines);
  asmw.PrintAll();

  CHECK(asmw.IsOccupiedWrapper(0));
  CHECK(!asmw.IsOccupiedWrapper(1));
  CHECK(asmw.IsOccupiedWrapper(20));
  CHECK(!asmw.IsOccupiedWrapper(21));

  uint16_t first = asmw.GetFirstEmptyWindowWithSizeWrapper(6);
  CHECK_EQ(first, 1);  // addr 0 is occupied, add 1 is free

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(19);
  CHECK_EQ(first, 1);  // addr 0 is occupied, add 1 is free

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(20);
  CHECK_EQ(first, 21);  // we can find 20 free cells at address 21

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(30);
  CHECK_EQ(first, 21);  // addr 20 is occupied, add 21 is free
}

TEST_CASE("check GetTotalSizeOfStringConsts") {
  map<int, string> lines {
    {1, ".str TEST \"abc\""},
  };

  AsmWrapper asmw;
  asmw.Process(lines);
  uint16_t sz = asmw.GetTotalSizeOfStringConstsWrapper();
  CHECK_EQ(sz, 4);
}

TEST_CASE("check GetTotalSizeOfStringConsts * MANY STRINGS") {
  map<int, string> lines {
    {1, ".str TEST \"test\""},   // 5
    {2, ".str ONE \"one\""},     // 4
    {3, ".str TWO \"two\""},     // 4
    {4, ".str THREE \"three\""}, // 6
  };                             // == 19

  AsmWrapper asmw;
  asmw.Process(lines);
  uint16_t sz = asmw.GetTotalSizeOfStringConstsWrapper();
  CHECK_EQ(sz, 19);
}

TEST_CASE("check GetTotalSizeOfDBConsts") {
  map<int, string> lines {
    {1, ".db TEST 123"},
  };

  AsmWrapper asmw;
  asmw.Process(lines);
  uint16_t sz = asmw.GetTotalSizeOfDBConstsWrapper();
  CHECK_EQ(sz, 1);
}

TEST_CASE("check GetTotalSizeOfDBConsts * MANY DB CONSTS") {
  map<int, string> lines {
    {1, ".db TEST 123"},       // 1 word
    {2, ".db T1 1, 2, 3, 4"},  // 4 words
    {3, ".db T2 3, 2"},        // 2 words
  };                           // 7 words

  AsmWrapper asmw;
  asmw.Process(lines);
  uint16_t sz = asmw.GetTotalSizeOfDBConstsWrapper();
  CHECK_EQ(sz, 7);
}

TEST_CASE("check GetTotalSizeOfDWConsts") {
  map<int, string> lines {
    {1, ".dw TEST 123"},       // 1
  };

  AsmWrapper asmw;
  asmw.Process(lines);
  uint16_t sz = asmw.GetTotalSizeOfDWConstsWrapper();
  CHECK_EQ(sz, 1);
}


TEST_CASE("check GetTotalSizeOfDWConsts * MANY DW CONSTS") {
  map<int, string> lines {
    {1, ".dw TEST 123"},       // 1
    {2, ".dw W1 1024, 2048"},  // 2
    {3, ".dw W2 32768, 2"},    // 2
  };                           // 5

  AsmWrapper asmw;
  asmw.Process(lines);
  uint16_t sz = asmw.GetTotalSizeOfDWConstsWrapper();
  CHECK_EQ(sz, 5);
}

TEST_CASE("check GetTotalSizeOf**Consts") {
  map<int, string> lines {
    {1, ".str S1 \"test string\""},  // 12
    {2, ".db B1 111, 222, 33"},      // 3
    {3, ".dw W2 32768, 2"},          // 2
  };

  AsmWrapper asmw;
  asmw.Process(lines);
  uint16_t sz1 = asmw.GetTotalSizeOfStringConstsWrapper();
  CHECK_EQ(sz1, 12);
  uint16_t sz2 = asmw.GetTotalSizeOfDBConstsWrapper();
  CHECK_EQ(sz2, 3);
  uint16_t sz3 = asmw.GetTotalSizeOfDWConstsWrapper();
  CHECK_EQ(sz3, 2);
}
