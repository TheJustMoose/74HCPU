#include <doctest.h>
#include <iomanip>
#include <iostream>
#include <optional>

#include "assm.h"
#include "error_collector.h"

using namespace std;

class AsmWrapper: public Assembler {
 public:
  AsmWrapper() = default;
  void OutCodeWrapper(vector<uint16_t>& code) {
    OutCode(code, true);
  }

  void OutCodeAndDebugInfoWrapper(vector<uint16_t>& code) {
    OutCode(code);
  }

  bool IsOccupiedWrapper(uint16_t addr) {
    return IsOccupied(addr);
  }

  bool OccupyItWrapper(uint16_t addr, uint16_t size) {
    return OccupyIt(addr, size);
  }

  optional<uint16_t> GetFirstEmptyWindowWithSizeWrapper(uint16_t size) {
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

  REQUIRE_EQ(code.size(), 4);
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
  asmw.OutCodeAndDebugInfoWrapper(code);

  //           0000 + DBGI + addr of str + 'S' + 0 +
  //           addr + 'CNSTS' + 0 + addr + 'PTR1' + 0 + addr + 'PTR2' + 0
  size_t dbg_info_size = 4 + 4 + 1 + 1 + 1 +
                         1 + 5 + 1 + 1 + 4 + 1 + 1 + 4 + 1;

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
  CHECK_EQ(code[20], 6);  // addr of CNSTS
  CHECK_EQ(code[21], 'C');
  CHECK_EQ(code[22], 'N');
  CHECK_EQ(code[23], 'S');
  CHECK_EQ(code[24], 'T');
  CHECK_EQ(code[25], 'S');
  CHECK_EQ(code[26], 0);  // ASCII Z

  CHECK_EQ(code[27], 10); // addr of PTR1
  CHECK_EQ(code[28], 'P');
  CHECK_EQ(code[29], 'T');
  CHECK_EQ(code[30], 'R');
  CHECK_EQ(code[31], '1');
  CHECK_EQ(code[32], 0);  // ASCII Z

  CHECK_EQ(code[33], 11); // addr of PTR2
  CHECK_EQ(code[34], 'P');
  CHECK_EQ(code[35], 'T');
  CHECK_EQ(code[36], 'R');
  CHECK_EQ(code[37], '2');
  CHECK_EQ(code[38], 0);  // ASCII Z

  CHECK_EQ(code[39], 2);
  CHECK_EQ(code[40], 'S');
  CHECK_EQ(code[41], 0);
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
}

TEST_CASE("check occupied_addresses_ bitset (empty)") {
  map<int, string> lines {
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  optional<uint16_t> first = asmw.GetFirstEmptyWindowWithSizeWrapper(1);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 0);
}

TEST_CASE("check occupied_addresses_ bitset (one block at addr 0)") {
  map<int, string> lines {
    {1, ".db BLOCK 1, 2, 3, 4"},
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  optional<uint16_t> first = asmw.GetFirstEmptyWindowWithSizeWrapper(1);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 4);
}

TEST_CASE("check occupied_addresses_ bitset (two blocks near)") {
  map<int, string> lines {
    {1, "STOP"},     // address 0
    {2, ".org 10"},  // address 10
    {3, "STOP"},     // address 10
  };

  AsmWrapper asmw;
  asmw.Process(lines);
  asmw.PrintAll();

  CHECK(asmw.IsOccupiedWrapper(0));   // first STOP command
  CHECK(!asmw.IsOccupiedWrapper(1));
  CHECK(!asmw.IsOccupiedWrapper(2));
  CHECK(!asmw.IsOccupiedWrapper(3));
  CHECK(!asmw.IsOccupiedWrapper(4));
  CHECK(!asmw.IsOccupiedWrapper(5));
  CHECK(!asmw.IsOccupiedWrapper(6));
  CHECK(!asmw.IsOccupiedWrapper(7));
  CHECK(!asmw.IsOccupiedWrapper(8));
  CHECK(!asmw.IsOccupiedWrapper(9));  // we have 9 free addresses in a row
  CHECK(asmw.IsOccupiedWrapper(10));  // last STOP command
  CHECK(!asmw.IsOccupiedWrapper(11));
  CHECK(!asmw.IsOccupiedWrapper(12));

  cout << dec;  // doctest does not want to switch the number base formatting to decimal (base 10)

  // Hint: a window means several free addresses in a row.
  optional<uint16_t> first = asmw.GetFirstEmptyWindowWithSizeWrapper(6);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 1);  // addr 0 and 10 are occupied, add 1, 2, 3, 4, 5, 6, 7, 8, 9 is free

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(7);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 1);  // addr 0 and 10 are occupied, add we have 9 free cells between them

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(8);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 1);  // addr 0 and 10 are occupied, add we have 9 free cells between them

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(9);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 1);  // addr 0 and 10 are occupied, add we have 9 free cells between them

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(10);  // Okay, asm, I need free window with size 10
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 11);  // addr 0 and 10 are occupied, add 1 is free, but window size (==9) is too small

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(17);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 11);  // addr 0 and 10 are occupied, add 1 is free

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(19);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 11);  // addr 0 and 10 are occupied, add 1 is free

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(20);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 11);  // we can find 20 free cells at address 11

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(30);
  CHECK(first.has_value());
  CHECK_EQ(first.value(), 11);  // addr 10 is occupied, but 11 is free

  asmw.OccupyItWrapper(11, 3);  // we need 3 memory cells started at address 11
  CHECK(asmw.IsOccupiedWrapper(11));
  CHECK(asmw.IsOccupiedWrapper(12));
  CHECK(asmw.IsOccupiedWrapper(13));
  CHECK(!asmw.IsOccupiedWrapper(14));  // free address again

  first = asmw.GetFirstEmptyWindowWithSizeWrapper(65535);
  CHECK(!first.has_value());  // we should get nullopt == no memory
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

TEST_CASE("check first word of binary") {
  map<int, string> lines {
    {1, "entry: clr  r0"},
    {2, "mov  YL, LO(Hello)"},
    {3, "mov  YH, HI(Hello)"},
    {4, ".str Hello \"Test string!\""},
  };

  AsmWrapper asmw;
  asmw.Process(lines);

  bool fatal {false};
  string el = ErrorCollector::GetInstance().get(1, fatal);
  CHECK(fatal);
  CHECK_EQ(el.find("Error"), 0);

  el = ErrorCollector::GetInstance().get(2, fatal);
  CHECK(!fatal);  // just message, no fatal error
  CHECK_EQ(el.find("Error"), string::npos);
}
