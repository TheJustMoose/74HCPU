#include "commands.h"

#include <cstdint>
#include <iostream>

#include "cpu.h"
#include "offset2int.h"

using namespace std;

uint8_t ArithmCmd::DstVal() {
  return cpu_->ActiveRegsBank()[Dst()];
}

uint8_t ArithmCmd::SrcVal() {
  if (IsConst())
    return Const();

  uint8_t res = cpu_->ActiveRegsBank()[Src()];
  if (NeedToZeroHighNibble())
    res &= 0x0F;
  if (NeedToZeroLowNibble())
    res &= 0xF0;

  if (InvHi())
    res ^= 0xF0;
  if (InvLo())
    res ^= 0x0F;

  return res;
}

void ArithmCmd::Execute() {
  cpu_->ActiveRegsBank()[Dst()] = Calculate(DstVal(), SrcVal());
}

string ArithmCmd::Params() {
  const char** names = cpu_->ActiveRegsNames();
  string rsrc = IsConst() ? to_string(Const()) : names[Src()];
  return string(" ") + names[Dst()] + string(", ") + rsrc;
}

uint8_t AddCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d + s;
  cpu_->flags[flags::CF] = (d + s) > 255;
  cpu_->flags[flags::ZF] = res == 0;
  return res;
}

uint8_t AddcCmd::Calculate(uint8_t d, uint8_t s) {
  bool cf = cpu_->flags[flags::CF];
  cf |= ForceCF();
  uint8_t res = d + s + cf;
  cpu_->flags[flags::CF] = (d + s + cf) > 255;
  cpu_->flags[flags::ZF] = res == 0;
  return res;
}

uint8_t AndCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d & s;
  cpu_->flags[flags::CF] = false;
  cpu_->flags[flags::ZF] = res == 0;
  return res;
}

uint8_t OrCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d | s;
  cpu_->flags[flags::CF] = false;
  cpu_->flags[flags::ZF] = res == 0;
  return res;
}

uint8_t XorCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d ^ s;
  cpu_->flags[flags::CF] = false;
  cpu_->flags[flags::ZF] = res == 0;
  return res;
}

void MulCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  uint16_t res = d * s;
  cpu_->flags[flags::CF] = false;
  cpu_->flags[flags::ZF] = res == 0;

  cpu_->ActiveRegsBank()[DstLowReg()] = res & 0xFF;
  cpu_->ActiveRegsBank()[DstHighReg()] = (res >> 8) & 0xFF;
}

uint8_t MovCmd::Calculate(uint8_t d, uint8_t s) {
  return s;
}

uint8_t UnaryCmd::DstVal() {
  return cpu_->ActiveRegsBank()[Dst()];
}

string UnaryCmd::Params() {
  string res = UnoNames[Type()];
  res += " ";
  res += cpu_->ActiveRegsNames()[Dst()];
  return res;
}

void InvCmd::Execute() {
  cpu_->ActiveRegsBank()[Dst()] = DstVal() ^ 0xFF;
}

void SwapCmd::Execute() {
  cpu_->ActiveRegsBank()[Dst()] =
      ((DstVal() & 0x0F) << 4) + ((DstVal() & 0xF0) >> 4);
}

void LsrCmd::Execute() {
  uint8_t val = DstVal();
  cpu_->flags[flags::CF] = val & 0x01;
  val >>= 1;
  cpu_->ActiveRegsBank()[Dst()] = val;
}

void LsrcCmd::Execute() {
  uint8_t val = DstVal();
  bool cf = cpu_->flags[flags::CF];
  cpu_->flags[flags::CF] = val & 0x01;
  val >>= 1;
  val |= (cf ? 0x80 : 0x00);
  cpu_->ActiveRegsBank()[Dst()] = val;
}

string MemoryCmd::Suffix() {
  string suffix;
  if (AutoDec())
    suffix += "D";
  if (AutoInc())
    suffix += "I";
  int offs = OffsetToInt(Offs());
  if (offs) {
    suffix += " + ";
    suffix += to_string(offs);  // offs may have value from -8 to +7 so I have to convert it to "int"
  }
  return suffix;
}

string LoadFromMemoryCmd::Params() {
  return string(" ") + cpu_->ActiveRegsNames()[Reg()] + string(", ") + PtrNames[Ptr()] + Suffix();
}

void LoadFromMemoryCmd::Execute() {
  uint16_t ptr = cpu_->GetPair(Ptr()) + OffsetToInt(Offs());
  if (V()) {
    uint16_t val = cpu_->ReadVRAM(ptr);
    cpu_->ActiveRegsBank()[LowReg()] = val & 0xFF;
    cpu_->ActiveRegsBank()[HighReg()] = (val >> 8) & 0xFF;
  } else {
    uint8_t val = cpu_->ReadRAM(ptr);
    cpu_->ActiveRegsBank()[Reg()] = val;
  }

  if (AutoInc())
    cpu_->IncPair(Ptr());
  if (AutoDec())
    cpu_->DecPair(Ptr());
}

string StoreToMemoryCmd::Params() {
  string pref;
  if (V())
    pref += "V";
  pref += " ";
  return pref + PtrNames[Ptr()] + Suffix() + string(", ") + cpu_->ActiveRegsNames()[Reg()];
}

void StoreToMemoryCmd::Execute() {
  uint16_t ptr = cpu_->GetPair(Ptr()) + OffsetToInt(Offs());
  if (V()) {
    uint16_t val = cpu_->ActiveRegsBank()[HighReg()];
    val <<= 8;
    val |= cpu_->ActiveRegsBank()[LowReg()];
    cpu_->WriteVRAM(ptr, val);
  } else {
    uint8_t val = cpu_->ActiveRegsBank()[Reg()];
    cpu_->WriteRAM(ptr, val);
  }

  if (AutoInc())
    cpu_->IncPair(Ptr());
  if (AutoDec())
    cpu_->DecPair(Ptr());
}

void LpmCmd::Execute() {
  uint16_t ptr = cpu_->GetPair(Ptr());
  uint16_t val = ptr < cpu_->rom.size() ? cpu_->rom[ptr] : 0xFFFF;
  if (W()) {
    cpu_->ActiveRegsBank()[DstLowReg()] = static_cast<uint8_t>(val & 0xFF);
    cpu_->ActiveRegsBank()[DstHighReg()] = static_cast<uint8_t>((val >> 8) & 0xFF);
  } else {
    cpu_->ActiveRegsBank()[Dst()] = static_cast<uint8_t>(val & 0xFF);
  }
  if (AutoInc())
    cpu_->IncPair(Ptr());
  if (AutoDec())
    cpu_->DecPair(Ptr());
}

string InputPortCmd::Params() {
  return string(" ") + cpu_->ActiveRegsNames()[Reg()] + string(", PINS") + to_string(Port());
}

void InputPortCmd::Execute() {
  uint8_t rdst = cpu_->pins[Port()];
  cpu_->ActiveRegsBank()[Reg()] = rdst;
}

uint8_t OutputPortCmd::SrcVal() {
  return IsConst() ? Const() : cpu_->ActiveRegsBank()[Reg()];
}

string OutputPortCmd::Params() {
  const char** names = cpu_->ActiveRegsNames();
  string rsrc = IsConst() ? to_string(Const()) : names[Reg()];
  return string(" PORT") + to_string(Port()) + string(", ") + rsrc;
}

void OutputPortCmd::Execute() {
  if (IsConst())
    cpu_->ports[Port()] = Const();
  else
    cpu_->ports[Port()] = cpu_->ActiveRegsBank()[Reg()];
}

uint8_t CmpCmd::DstVal() {
  return cpu_->ActiveRegsBank()[Dst()];
}

uint8_t CmpCmd::SrcVal() {
  return IsConst() ? Const() : cpu_->ActiveRegsBank()[Src()];
}

Relation CmpCmd::Compare(uint8_t left, uint8_t right) {
  if (left < right) {
    return { true, false, false };
  } else if (left == right) {
    return { false, true, false };
  } else {
    return { false, false, true };
  }
}

void CmpCmd::Execute() {
  auto [lf, ef, gf] = Compare(DstVal(), SrcVal());
  cpu_->flags[flags::LF] = lf;
  cpu_->flags[flags::EF] = ef;
  cpu_->flags[flags::GF] = gf;
}

string CmpCmd::Params() {
  const char** names = cpu_->ActiveRegsNames();
  string rsrc = IsConst() ? to_string(cmd_ & 0xFF) : names[Src()];
  return string(" ") + names[Dst()] + string(", ") + rsrc;
}

void CmpcCmd::Execute() {
  // check previous command
  if (!cpu_->flags[flags::EF])  // ok, high byte is different
    return;  // nothing to do here

  // Let's compare this (low) byte
  auto [lf, ef, gf] = Compare(DstVal(), SrcVal());
  cpu_->flags[flags::LF] = lf;
  cpu_->flags[flags::EF] = ef;
  cpu_->flags[flags::GF] = gf;
}
