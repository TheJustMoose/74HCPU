#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "commands.h"
#include "cpu.h"
#include "names.h"
#include "offset2int.h"

using namespace std;

TEST_CASE("test OffsetToInt") {
  // 4 bit two's complement value
  CHECK(OffsetToInt(0) == 0);
  CHECK(OffsetToInt(1) == 1);
  CHECK(OffsetToInt(7) == 7);
  CHECK(OffsetToInt(8) == -8);
  CHECK(OffsetToInt(15) == -1);
  CHECK(OffsetToInt(16) == 0);
  CHECK(OffsetToInt(255) == 0);
}

TEST_CASE("test ByteOffsetToInt") {
  // 8 bit two's complement value
  CHECK(ByteOffsetToInt(0) == 0);
  CHECK(ByteOffsetToInt(1) == 1);
  CHECK(ByteOffsetToInt(127) == 127);
  CHECK(ByteOffsetToInt(128) == -128);
  CHECK(ByteOffsetToInt(255) == -1);
}

TEST_CASE("test names") {
  CHECK(std::size(RegNames) == 8);  // 8 registers
  CHECK(std::size(OpNames) == 16);  // and 16 commands
}

TEST_CASE("test Cmds") {
  CPU cpu;
  AddCmd ac(0x0164, &cpu);  // ADD R0, 100
  // test AddCmd::Calculate
  CHECK( ac.Calculate(0, 0) == 0 );
  CHECK( !cpu.Flags[flags::CF] );
  CHECK( cpu.Flags[flags::ZF] );

  CHECK( ac.Calculate(100, 100) == 200 );
  CHECK( !cpu.Flags[flags::CF] );
  CHECK( !cpu.Flags[flags::ZF] );

  CHECK( ac.Calculate(100, 200) == 44 );
  CHECK( cpu.Flags[flags::CF] );
  CHECK( !cpu.Flags[flags::ZF] );

  CHECK( ac.Calculate(100, 156) == 0 );
  CHECK( cpu.Flags[flags::CF] );
  CHECK( cpu.Flags[flags::ZF] );

  // test AddCmd::Execute
  cpu.RegsBank0[0] = 100;   // MOV R0, 100
  ac.Execute();             // ADD R0, 100
  CHECK( cpu.RegsBank0[0] == 200 );

  // test AddcCmd::Execute
  cpu.RegsBank0[0] = 100;    // MOV R0, 100
  cpu.Flags[flags::CF] = true;
  AddcCmd acc(0x1164, &cpu); // ADDC R0, 100
  acc.Execute();
  CHECK( cpu.RegsBank0[0] == 201 );  // 100 + 100 + CF

  // test AndCmd::Execute
  cpu.RegsBank0[0] = 0x55;   // MOV R0, 0x55
  AndCmd anc(0x21AA, &cpu);  // AND R0, 0xAA
  anc.Execute();
  CHECK( cpu.RegsBank0[0] == 0 );

  // test OrCmd::Execute
  cpu.RegsBank0[0] = 0x55;   // MOV R0, 0x55
  OrCmd oc(0x31AA, &cpu);    // OR R0, 0xAA
  oc.Execute();
  CHECK( cpu.RegsBank0[0] == 0xFF );

  // test XorCmd::Execute
  cpu.RegsBank0[0] = 0x55;   // MOV R0, 0x55
  XorCmd xoc(0x415A, &cpu);  // XOR R0, 0x5A
  xoc.Execute();
  CHECK( cpu.RegsBank0[0] == 0x0F );

  // test MulCmd::Execute
  MulCmd muc(0x515A, &cpu);  // MUL R0, 0x5A
  cpu.RegsBank0[0] = 0;      // MOV R0, 0
  muc.Execute();
  CHECK( cpu.RegsBank0[0] == 0 );
  CHECK( cpu.RegsBank0[1] == 0 );
  cpu.RegsBank0[0] = 1;      // MOV R0, 1
  muc.Execute();
  CHECK( cpu.RegsBank0[0] == 0x5A );
  CHECK( cpu.RegsBank0[1] == 0x00 );
  cpu.RegsBank0[0] = 4;      // MOV R0, 4
  muc.Execute();
  CHECK( cpu.RegsBank0[0] == 0x68 );  // 5Ah * 4 == 168h
  CHECK( cpu.RegsBank0[1] == 0x01 );

  // test MovCmd::Execute
  cpu.RegsBank0[0] = 0;      // MOV R0, 0
  MovCmd mc(0x715A, &cpu);   // MOV R0, 0x5A
  mc.Execute();
  CHECK( cpu.RegsBank0[0] == 0x5A );
}

TEST_CASE("test UnoCmds") {
  //CPU cpu;
  //InvCnd
}

TEST_CASE("test Ptrs Arithm") {
  CPU cpu;
  cpu.SetPair(0, 0x1234);
  CHECK( cpu.RegsBank1[0] == 0x34 );
  CHECK( cpu.RegsBank1[1] == 0x12 );
  CHECK( cpu.GetPair(0) == 0x1234);
  CHECK( cpu.GetPair(-1) == 0x1234);
}

TEST_CASE("test Memory Cmds") {
  CPU cpu;

  cpu.RegsBank0[0] = 10;             // MOV R0, 10
  cpu.RegsBank1[0] = 100;            // MOV XL, 100
  cpu.RegsBank1[1] = 0;              // MOV XH, 0
  cpu.RAM[100] = 0;                  // *100 = 0
  MemoryCmd mcmd(0xC000, &cpu);      // ST X, R0
  mcmd.Execute();
  CHECK( cpu.RAM[100] == 10 );       // *100 == 10

  cpu.RegsBank0[0] = 20;             // MOV R0, 20
  cpu.RegsBank1[0] = 100;            // MOV XL, 100
  cpu.RegsBank1[1] = 0;              // MOV XH, 0
  cpu.RAM[100] = 0;                  // *100 = 0
  MemoryCmd mcmd2(0xC010, &cpu);     // ST X+, R0 (with autoinc)
  mcmd2.Execute();
  CHECK( cpu.RAM[100] == 20 );       // *100 == 20
  CHECK( cpu.RegsBank1[0] == 101 );  // autoinc work!
}
