#include <doctest.h>

#include "assm.h"

using namespace std;

TEST_CASE("check names getter") {
  CHECK(Names::RegFromName("Rx") == rUnkReg);

  CHECK(Names::RegFromName("R0") == rR0);
  CHECK(Names::RegFromName("R1") == rR1);
  CHECK(Names::RegFromName("R2") == rR2);
  CHECK(Names::RegFromName("R3") == rR3);
  CHECK(Names::RegFromName("R4") == rR4);
  CHECK(Names::RegFromName("R5") == rR5);
  CHECK(Names::RegFromName("R6") == rR6);
  CHECK(Names::RegFromName("R7") == rR7);

  CHECK(Names::RegFromName("XL") == rXL);
  CHECK(Names::RegFromName("XH") == rXH);
  CHECK(Names::RegFromName("YL") == rYL);
  CHECK(Names::RegFromName("YH") == rYH);
  CHECK(Names::RegFromName("SPL") == rSPL);
  CHECK(Names::RegFromName("SPH") == rSPH);
  CHECK(Names::RegFromName("VL") == rVL);
  CHECK(Names::RegFromName("VH") == rVH);

  bool inc, dec;
  CHECK(Names::PtrFromName("X", &inc, &dec) == rX);
  CHECK(!inc); CHECK(!dec);

  CHECK(Names::PtrFromName("XI", &inc, &dec) == rX);
  CHECK(inc); CHECK(!dec);  // post increment

  CHECK(Names::PtrFromName("XD", &inc, &dec) == rX);
  CHECK(!inc); CHECK(dec);  // post decrement

  CHECK(Names::PtrFromName("Y", &inc, &dec) == rY);
  CHECK(!inc); CHECK(!dec);

  CHECK(Names::PtrFromName("YI", &inc, &dec) == rY);
  CHECK(inc); CHECK(!dec);  // post increment

  CHECK(Names::PtrFromName("YD", &inc, &dec) == rY);
  CHECK(!inc); CHECK(dec);  // post decrement

  CHECK(Names::PtrFromName("SP", &inc, &dec) == rSP);
  CHECK(!inc); CHECK(!dec);

  CHECK(Names::PtrFromName("SPI", &inc, &dec) == rSP);
  CHECK(inc); CHECK(!dec);  // post increment

  CHECK(Names::PtrFromName("SPD", &inc, &dec) == rSP);
  CHECK(!inc); CHECK(dec);  // post decrement

  CHECK(Names::PtrFromName("V", &inc, &dec) == rV);
  CHECK(!inc); CHECK(!dec);

  CHECK(Names::PtrFromName("VI", &inc, &dec) == rV);
  CHECK(inc); CHECK(!dec);  // post increment

  CHECK(Names::PtrFromName("VD", &inc, &dec) == rV);
  CHECK(!inc); CHECK(dec);  // post decrement
}
