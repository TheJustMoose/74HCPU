#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <string>

#include "assm.h"
#include "str_util.h"

using namespace std;

TEST_CASE("check arithm COPs") {
  CodeLine cl1(1, "NOP");
  CHECK(cl1.GenerateMachineCode() == 0xFFFF);

  // 0000 000 0 000 00000
  CodeLine cl2(1, "ADD R0, R0");
  CHECK(cl2.GenerateMachineCode() == 0x0000);

  // 0000 001 0 010 00000
  CodeLine cl3(1, "ADD R1, R2");
  CHECK(cl3.GenerateMachineCode() == 0x0240);

  // 0000 010 0 011 00000
  CodeLine cl4(1, "ADD R2, R3");
  CHECK(cl4.GenerateMachineCode() == 0x0460);

  // 0001 010 0 011 00000
  CodeLine cl5(1, "ADDC R2, R3");
  CHECK(cl5.GenerateMachineCode() == 0x1460);

  // 0010 011 0 100 00000
  CodeLine cl6(1, "AND R3, R4");
  CHECK(cl6.GenerateMachineCode() == 0x2680);

  // 0011 100 0 101 00000
  CodeLine cl7(1, "OR R4, R5");
  CHECK(cl7.GenerateMachineCode() == 0x38A0);

  // 0100 101 0 110 00000
  CodeLine cl8(1, "XOR R5, R6");
  CHECK(cl8.GenerateMachineCode() == 0x4AC0);

  // 0101 110 0 111 00000
  CodeLine cl9(1, "MUL R6, R7");
  CHECK(cl9.GenerateMachineCode() == 0x5CE0);

  // 0110 111 0 000 00000
  CodeLine clB(1, "INV R7");
  CHECK(clB.GenerateMachineCode() == 0x6E00);

  // 0110 111 0 001 00000
  CodeLine clA(1, "SWAP R7");
  CHECK(clA.GenerateMachineCode() == 0x6E20);

  // 0110 111 0 010 00000
  CodeLine clC(1, "LSR R7");
  CHECK(clC.GenerateMachineCode() == 0x6E40);

  // 0110 111 0 011 00000
  CodeLine clD(1, "LSRC R7");
  CHECK(clD.GenerateMachineCode() == 0x6E60);

  // 0111 000 0 111 00000
  CodeLine clE(1, "MOV R0, R7");
  CHECK(clE.GenerateMachineCode() == 0x70E0);
}

TEST_CASE("check Inversion COPs") {
  // |SUB/ADDC|  DST |C| SRC |F|Z|z|I|i|
  //     0001    001  0  010  1   0011
  // SUB A, B == ADD A, ~B + 1
  CodeLine cl1(1, "SUB R1, R2");
  //CHECK(cl1.GenerateMachineCode() == 0x1253);
}

TEST_CASE("check Nibble (4 bit) COPs") {
  // |   ADD |  DST |C| SRC |-|Z|z|I|i|
  //    0000    000  0  000  0   1000
  CodeLine cl1(1, "ADD R0, L(R0)");  // ADD low nibble of [right] register -> zero high nibble
  CHECK(cl1.GenerateMachineCode() == 0x0008);

  //    0000    000  0  000  0   0100
  CodeLine cl2(1, "ADD R0, H(R0)");  // ADD high nibble of [right] register -> zero low nibble
  CHECK(cl2.GenerateMachineCode() == 0x0004);

  //    0000    000  0  001  0   0011
  CodeLine cl3(1, "ADD R0, ~R1");  // ADD inverted register R1 to R0
  CHECK(cl3.GenerateMachineCode() == 0x0023);

  //    0000    000  0  001  0   0010
  CodeLine cl4(1, "ADD R0, `R1");  // invert high nibble and ADD register R1 to R0
  CHECK(cl4.GenerateMachineCode() == 0x0022);

  //    0000    000  0  001  0   0001
  CodeLine cl5(1, "ADD R0, 'R1");  // invert low nibble and ADD register R1 to R0
  CHECK(cl5.GenerateMachineCode() == 0x0021);

  //    0001    000  0  001  1   0011
  CodeLine cl6(1, "ADDC R0, ~R1 + 1");  // ADDC inverted register R1 to R0 with CF==1
  CHECK(cl6.GenerateMachineCode() == 0x1033);
}

TEST_CASE("check SUB COPs") {
  //    0001    000  0  001  1   0011
  CodeLine cl1(1, "SUB R0, R1");  // will be converted to: ADDC R0, ~R1 + 1
  CHECK(cl1.GenerateMachineCode() == 0x1033);
}

TEST_CASE("check ROM COPs") {
  // LPM  DST W SR DU OFST
  // 1000 000 0 00 00 0000
  CodeLine cl1(1, "LPM R0, X");
  CHECK(cl1.GenerateMachineCode() == 0x8000);

  // 1000 101 0 00 00 0000
  CodeLine cl2(1, "LPM R5, X");
  CHECK(cl2.GenerateMachineCode() == 0x8A00);

  // 1000 000 0 01 00 0000
  CodeLine cl3(1, "LPM R0, Y");
  CHECK(cl3.GenerateMachineCode() == 0x8040);

  // 1000 000 0 10 00 0000
  CodeLine cl4(1, "LPM R0, Z");  // R0 = *Z;
  CHECK(cl4.GenerateMachineCode() == 0x8080);

  // 1000 000 0 11 00 0000
  CodeLine cl5(1, "LPM R0, SP");
  CHECK(cl5.GenerateMachineCode() == 0x80C0);

  // 1000 110 0 11 00 0001
  CodeLine cl6(1, "LPM R6, SP+1");  // R6 = *(SP + 1);
  CHECK(cl6.GenerateMachineCode() == 0x8CC1);

  // 1000 110 0 11 00 0111
  CodeLine cl7(1, "LPM R6, SP+7");  // R6 = *(SP + 7);
  CHECK(cl7.GenerateMachineCode() == 0x8CC7);

  // 1000 110 0 11 00 1111
  CodeLine cl8(1, "LPM R6, SP-1");
  CHECK(cl8.GenerateMachineCode() == 0x8CCF);

  // 1000 110 0 11 00 1110
  CodeLine cl9(1, "LPM R6, SP-2");
  CHECK(cl9.GenerateMachineCode() == 0x8CCE);

  // 1000 110 0 11 00 1001
  CodeLine clA(1, "LPM R6, SP-7");
  CHECK(clA.GenerateMachineCode() == 0x8CC9);

  // 1000 110 0 11 00 1000
  CodeLine clB(1, "LPM R6, SP-8");
  CHECK(clB.GenerateMachineCode() == 0x8CC8);

  // LPM  DST W SR DU OFST
  // 1000 101 0 00 01 0000
  CodeLine clC(1, "LPM R5, XI");  // R5 = *X++; in C++ notation (Load with post-increment)
  CHECK(clC.GenerateMachineCode() == 0x8A10);

  // 1000 101 0 00 10 0000
  CodeLine clD(1, "LPM R5, XD");  // R5 = *X--; in C++ notation (Load with post-decrement)
  CHECK(clD.GenerateMachineCode() == 0x8A20);

  // 1000 101 0 00 10 0001
  CodeLine clE(1, "LPM R5, XD+1");  // R5 = *(X + 1); X--;  in C++ notation (Load with post-decrement and displacement)
  CHECK(clE.GenerateMachineCode() == 0x8A21);
}

TEST_CASE("check RAM COPs") {
  // LD   DST - SR DU OFST
  // 1001 000 0 00 00 0000
  CodeLine cl1(1, "LD R0, X");
  CHECK(cl1.GenerateMachineCode() == 0x9000);

  // ST   SRC - DS DU OFST
  // 1100 000 0 00 00 0000
  CodeLine cl2(1, "ST X, R0");
  CHECK(cl2.GenerateMachineCode() == 0xC000);

  // 1100 000 0 00 00 0101
  CodeLine cl3(1, "ST X+5, R0");
  CHECK(cl3.GenerateMachineCode() == 0xC005);

  // 1100 111 0 01 00 0000
  CodeLine cl4(1, "ST Y, R7");
  CHECK(cl4.GenerateMachineCode() == 0xCE40);
}

TEST_CASE("check stack COPs") {
  //.def push(r) ST SPD, r
  // ST   SRC - DS DU OFST
  // 1100 001 0 11 10 0000
  CodeLine cl1(1, "ST SPD, R1");
  CHECK(cl1.GenerateMachineCode() == 0xC2E0);
  //.def pop(r)  LD r, SPI+1
  // LD   DST - SR DU OFST
  // 1001 001 0 11 01 0001
  CodeLine cl2(2, "LD R1, SPI+1");
  CHECK(cl2.GenerateMachineCode() == 0x92D1);
}

TEST_CASE("check I/O COPs") {
  // IN   DST  PORT ZzIi
  // 1010 011 00010 0000
  CodeLine cl1(1, "IN R3, PINS1");
  CHECK(cl1.GenerateMachineCode() == 0xA620);

  // OUT | PORT |0| SRC |PRT|X|O|o|
  // 1011   110  0  111  00   000  // PORT4 has number 6 (00110), see port_names
  CodeLine cl2(1, "OUT PORT4, R7");
  CHECK(cl2.GenerateMachineCode() == 0xBCE0);

  // OUT | PORT |1|     CONST     |
  // 1011   110  1   0000 1010     // CPU_FLAGS == PORT4 has number 6 (00110), see port_names
  CodeLine cl3(1, "OUT CPU_FLAGS, 10");
  CHECK(cl3.GenerateMachineCode() == 0xBD0A);

  // OUT | PORT |0| SRC |PRT|X|O|o|
  // 1011   110  0  000  00   100  // PORT4 has number 6 (00110), see port_names
  CodeLine cl4(1, "TOGL PORT4, R0");
  CHECK(cl4.GenerateMachineCode() == 0xBC04);
}

TEST_CASE("check branches COPs") {
  // 1111 0010 00000000
  CodeLine cl1(1, "LBL: RET");
  CHECK(cl1.GenerateMachineCode() == 0xF200);
  // 1111 0000 00000000
  CodeLine cl2(2, "CALL LBL");  // fortunately, LBL address is 0
  CHECK(cl2.GenerateMachineCode() == 0xF000);

  // 1111 0001 00001010
  CodeLine cl3(1, "JMP LBL");
  // Try to set LBL offset (from current address)
  cl3.UpdateMachineCode( map<string, uint16_t> { {"LBL", 10} } );
  CHECK(cl3.GenerateMachineCode() == 0xF10A);

  // 1111 0001 11111111
  CodeLine cl4(1, "JMP LBL");
  cl4.SetAddress(0x0010u);
  // Try to set LBL offset (from current address)
  cl4.UpdateMachineCode( map<string, uint16_t> { { "LBL", 0x000Fu } } );
  CHECK(cl4.GenerateMachineCode() == 0xF1FF);  // jmp to previous instruction

  // 1111 0010 00000000
  CodeLine cl5(1, "RET");
  CHECK(cl5.GenerateMachineCode() == 0xF200);

  // 1111 0011 00000000
  CodeLine cl6(1, "RETI");
  CHECK(cl6.GenerateMachineCode() == 0xF300);

  // 1111 0100 00000000
  CodeLine cl7(1, "JL TST");
  CHECK(cl7.GenerateMachineCode() == 0xF400);

  // 1111 0101 00000000
  CodeLine cl8(1, "JE TST");
  CHECK(cl8.GenerateMachineCode() == 0xF500);

  // 1111 0110 00000000
  CodeLine cl9(1, "JNE TST");
  CHECK(cl9.GenerateMachineCode() == 0xF600);

  // 1111 0111 00000000
  CodeLine clA(1, "JG TST");
  CHECK(clA.GenerateMachineCode() == 0xF700);

  // 1111 1000 00000000
  CodeLine clB(1, "JZ TST");
  CHECK(clB.GenerateMachineCode() == 0xF800);

  // 1111 1001 00000000
  CodeLine clC(1, "JNZ TST");
  CHECK(clC.GenerateMachineCode() == 0xF900);

  // 1111 1010 00000000
  CodeLine clD(1, "JC TST");
  CHECK(clD.GenerateMachineCode() == 0xFA00);

  // 1111 1011 00000000
  CodeLine clE(1, "JNC TST");
  CHECK(clE.GenerateMachineCode() == 0xFB00);

  // 1111 1100 00000000
  CodeLine clF(1, "JHC TST");
  CHECK(clF.GenerateMachineCode() == 0xFC00);

  // 1111 1101 00000000
  CodeLine clG(1, "JNHC TST");
  CHECK(clG.GenerateMachineCode() == 0xFD00);

  // 1111 1110 00000000
  CodeLine clH(1, "AFCALL TST");
  CHECK(clH.GenerateMachineCode() == 0xFE00);

  // 1111 1111 11111111
  CodeLine clI(1, "NOP");
  CHECK(clI.GenerateMachineCode() == 0xFFFF);

  // 1111 1111 11111110
  CodeLine clJ(1, "STOP");
  CHECK(clJ.GenerateMachineCode() == 0xFFFE);
}

TEST_CASE("check jump distance") {
  // 1111 0001 00000000
  CodeLine cl1(1, "JMP L1");
  // Try to set L1 label offset (from current address)
  cl1.UpdateMachineCode( map<string, uint16_t> { {"L1", 0} } );  // no jmp (jmp to same command)
  CHECK(cl1.GenerateMachineCode() == 0xF100);

  // 1111 0001 11111111
  CodeLine cl2(1, "JMP L1");
  cl2.SetAddress(0x000Au);
  // Try to set L1 label offset (from current address)
  cl2.UpdateMachineCode( map<string, uint16_t> { { "L1", 0x0009u } } );  // jmp to previous cmd
  CHECK(cl2.GenerateMachineCode() == 0xF1FF);

  // 1111 0001 11111110
  CodeLine cl3(1, "JMP L1");
  cl3.SetAddress(0x000Au);
  // Try to set L1 label offset (from current address)
  cl3.UpdateMachineCode( map<string, uint16_t> { { "L1", 0x0008u } } );  // jmp two cmd back
  CHECK(cl3.GenerateMachineCode() == 0xF1FE);
}
