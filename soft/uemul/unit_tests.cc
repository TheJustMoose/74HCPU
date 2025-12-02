#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <cstdint>

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
  CHECK(std::size(PtrNames) == 4);  // 4 pointers
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
  CPU cpu;

  cpu.RegsBank0[0] = 0x81;   // MOV R0, 0x81
  InvCmd ic(0x6000, &cpu);
  ic.Execute();              // INV R0
  CHECK( cpu.RegsBank0[0] == 0x7E );

  cpu.RegsBank0[0] = 0x81;   // MOV R0, 0x81
  SwapCmd sc(0x6020, &cpu);
  sc.Execute();              // SWAP R0
  CHECK( cpu.RegsBank0[0] == 0x18 );

  cpu.RegsBank0[0] = 0x55;   // MOV R0, 0x55
  LsrCmd lc(0x6040, &cpu);
  lc.Execute();              // LSR R0
  CHECK( cpu.RegsBank0[0] == 0x2A );
  CHECK( cpu.Flags[flags::CF] );

  cpu.Flags[flags::CF] = true;
  cpu.RegsBank0[0] = 0x55;   // MOV R0, 0x55
  LsrcCmd lcc(0x6060, &cpu);
  lcc.Execute();             // LSRC R0
  CHECK( cpu.RegsBank0[0] == 0xAA );
  CHECK( cpu.Flags[flags::CF] );
}

TEST_CASE("test Ptrs Arithm") {
  CPU cpu;
  cpu.SetPair(0, 0x1234);
  CHECK( cpu.RegsBank1[0] == 0x34 );
  CHECK( cpu.RegsBank1[1] == 0x12 );
  CHECK( cpu.GetPair(0) == 0x1234);
  cpu.IncPair(0);
  CHECK( cpu.GetPair(0) == 0x1235);
  cpu.DecPair(0);
  CHECK( cpu.GetPair(0) == 0x1234);

  cpu.SetPair(1, 0x00FF);
  cpu.IncPair(1);
  CHECK( cpu.GetPair(1) == 0x0100);
  cpu.DecPair(1);
  CHECK( cpu.GetPair(1) == 0x00FF);

  cpu.SetPair(2, 0x0010);
  cpu.IncPair(2);
  CHECK( cpu.GetPair(2) == 0x0011);
  cpu.DecPair(2);
  CHECK( cpu.GetPair(2) == 0x0010);

  cpu.SetPair(3, 0xFFFF);
  cpu.IncPair(3);
  CHECK( cpu.GetPair(3) == 0x0000);
  cpu.DecPair(3);
  CHECK( cpu.GetPair(3) == 0xFFFF);

  // ptr are stored in different places
  CHECK( cpu.GetPair(0) != cpu.GetPair(1) );
  CHECK( cpu.GetPair(1) != cpu.GetPair(2) );
  CHECK( cpu.GetPair(2) != cpu.GetPair(3) );
  CHECK( cpu.GetPair(3) != cpu.GetPair(0) );
}

TEST_CASE("test Memory Cmds") {
  CPU cpu;
  // ST cmd
  cpu.RegsBank0[0] = 10;             // MOV R0, 10
  cpu.RegsBank1[0] = 100;            // MOV XL, 100
  cpu.RegsBank1[1] = 0;              // MOV XH, 0
  cpu.RAM[100] = 0;                  // *100 = 0
  StoreToMemoryCmd mcmd(0xC000, &cpu);  // ST X, R0
  mcmd.Execute();
  CHECK( cpu.RAM[100] == 10 );       // *100 == 10

  // ST cmd with pointer autoincrement
  cpu.RegsBank0[0] = 20;             // MOV R0, 20
  cpu.RegsBank1[0] = 100;            // MOV XL, 100
  cpu.RegsBank1[1] = 0;              // MOV XH, 0
  cpu.RAM[100] = 0;                  // *100 = 0
  StoreToMemoryCmd mcmd2(0xC010, &cpu);  // ST XI, R0 (with autoinc)
  mcmd2.Execute();
  CHECK( cpu.RAM[100] == 20 );       // *100 == 20
  CHECK( cpu.RegsBank1[0] == 101 );  // autoinc work!

  // ST cmd with pointer autoincrement and displacement
  cpu.RegsBank0[0] = 30;             // MOV R0, 30
  cpu.RegsBank1[0] = 100;            // MOV XL, 100
  cpu.RegsBank1[1] = 0;              // MOV XH, 0
  cpu.RAM[105] = 0;                  // *105 = 0
  StoreToMemoryCmd mcmd3(0xC015, &cpu);  // ST XI + 5, R0 (with autoinc)
  mcmd3.Execute();
  CHECK( cpu.RAM[105] == 30 );       // *105 == 30
  CHECK( cpu.RegsBank1[0] == 101 );  // autoinc work!
}

TEST_CASE("test Ports Cmds") {
  CPU cpu;
  cpu.RegsBank0[0] = 0x55;           // MOV R0, 0x55
  OutputPortCmd ocmd(0xB400, &cpu);  // OUT PORT1, R0
  CHECK( ocmd.PortLo() == 2 );
  CHECK( ocmd.PortHi() == 0 );
  CHECK( ocmd.Reg() == 0 );
  CHECK( ocmd.IsConst() == false );

  ocmd.Execute();
  CHECK( cpu.PORTS[2] == 0x55 );     // *1 == 0x55

  cpu.PINS[2] = 0x66;                // PINS1 has binary number 2
  InputPortCmd icmd(0xA220, &cpu);   // IN R1, PINS1
  icmd.Execute();
  CHECK( cpu.RegsBank0[1] == 0x66 ); // R1 == 0x66
}

TEST_CASE("test LPM Cmd") {
  CPU cpu;
  cpu.ROM.push_back(0x0110);
  cpu.ROM.push_back(0x0220);
  cpu.ROM.push_back(0x0330);
  cpu.ROM.push_back(0x0440);

  // read 1 byte from ROM into "low" register R4
  cpu.RegsBank1[0] = 1;              // MOV XL, 1
  cpu.RegsBank1[1] = 0;              // MOV XH, 0
  LpmCmd lcmd(0x8800, &cpu);         // LPM R4, X
  lcmd.Execute();
  CHECK( cpu.RegsBank0[4] == 0x20 ); // R4 == 0x20

  // read 1 byte from ROM into "high" register R5
  LpmCmd lcmd2(0x8A00, &cpu);        // LPM R5, X
  lcmd2.Execute();
  CHECK( cpu.RegsBank0[5] == 0x20 ); // R5 == 0x20

  // read 2 bytes from ROM into register pair R5:R4
  LpmCmd lcmd3(0x8B00, &cpu);        // LPMW R5, X
  lcmd3.Execute();
  CHECK( cpu.RegsBank0[4] == 0x20 ); // R4 == 0x20  // low byte register
  CHECK( cpu.RegsBank0[5] == 0x02 ); // R5 == 0x02  // high byte register

  // read 2 bytes from ROM into register pair R5:R4
  LpmCmd lcmd4(0x8900, &cpu);        // LPMW R4, X
  lcmd4.Execute();
  CHECK( cpu.RegsBank0[4] == 0x20 ); // R4 == 0x20  // low byte register
  CHECK( cpu.RegsBank0[5] == 0x02 ); // R5 == 0x02  // high byte register
  // so LPMW R4, X and LPMW R5, X have different opcodes but equal result
}

TEST_CASE("test LPM autoincrement") {
  CPU cpu;
  cpu.ROM.push_back(0x0110);

  cpu.RegsBank1[0] = 0;              // MOV XL, 0
  cpu.RegsBank1[1] = 0;              // MOV XH, 0
  LpmCmd lcmd(0x8810, &cpu);         // LPM R4, X+
  lcmd.Execute();
  CHECK( cpu.RegsBank0[4] == 0x10 ); // R4 == 0x20
  CHECK( cpu.RegsBank1[0] == 1 );    // XL == 1
}

TEST_CASE("test Find Debug Info") {
  CPU cpu;
  size_t data_start {0};
  CHECK( !cpu.FindDebugInfo(data_start) );

  // signature have size == 8, also we need at least one byte of data
  std::vector<uint16_t> ROM {0, 0, 0, 0, 'D', 'B', 'G', 'I', 0};
  cpu.ROM = ROM;
  CHECK( cpu.FindDebugInfo(data_start) );
  CHECK( data_start == 8 );  // index of first byte of data
}

TEST_CASE("test Read Debug Info") {
  CPU cpu;
  std::vector<uint16_t> ROM {0, 0, 0, 0, 'D', 'B', 'G', 'I', 0x1234, 'L', 'a', 'b', 'e', 'l', 0};
  cpu.ROM = ROM;
  cpu.ReadDebugInfo(8);  // yes, I know index of data
  CHECK( cpu.name_to_address.size() );
  CHECK( cpu.name_to_address.find("Label") != cpu.name_to_address.end() );
  CHECK( cpu.name_to_address["Label"] == 0x1234 );
}

//|   CMP |  DST |C| SRC |-|   -   | D0 1101 0000|+|
TEST_CASE("test Cmp command") {
  CPU cpu;
  cpu.RegsBank0[0] = 100;   // MOV R0, 100
  CmpCmd cc(0xD164, &cpu);  // CMP R0, 100  // D 1 6 4
  cc.Execute();
  CHECK( !cpu.Flags[flags::LF] );
  CHECK( cpu.Flags[flags::EF] );
  CHECK( !cpu.Flags[flags::GF] );

  cpu.RegsBank0[0] = 99;   // MOV R0, 99
  cc.Execute();
  CHECK( cpu.Flags[flags::LF] );
  CHECK( !cpu.Flags[flags::EF] );
  CHECK( !cpu.Flags[flags::GF] );

  cpu.RegsBank0[0] = 101;   // MOV R0, 101
  cc.Execute();
  CHECK( !cpu.Flags[flags::LF] );
  CHECK( !cpu.Flags[flags::EF] );
  CHECK( cpu.Flags[flags::GF] );
}
