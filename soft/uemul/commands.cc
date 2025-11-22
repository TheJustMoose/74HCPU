#include "commands.h"

#include <iostream>

#include "cpu.h"

using namespace std;

uint8_t ArithmCmd::DstVal() {
  return cpu_->ActiveRegsBank()[Dst()];
}

uint8_t ArithmCmd::SrcVal() {
  return IsConst() ? Const() : cpu_->ActiveRegsBank()[Src()];
}

void ArithmCmd::Execute() {
  cpu_->ActiveRegsBank()[Dst()] = Calculate(DstVal(), SrcVal());
}

string ArithmCmd::Params() {
  string rsrc = IsConst() ? to_string(cmd_ & 0xFF) : RegNames[Src()];
  return string(" ") + RegNames[Dst()] + string(", ") + rsrc;
}

uint8_t AddCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d + s;
  cpu_->Flags[flags::CF] = (d + s) > 255;
  cpu_->Flags[flags::ZF] = res == 0;
  return res;
}

uint8_t AddcCmd::Calculate(uint8_t d, uint8_t s) {
  bool cf = cpu_->Flags[flags::CF];
  uint8_t res = d + s + cf;
  cpu_->Flags[flags::CF] = (d + s + cf) > 255;
  cpu_->Flags[flags::ZF] = res == 0;
  return res;
}

uint8_t AndCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d & s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  return res;
}

uint8_t OrCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d | s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  return res;
}

uint8_t XorCmd::Calculate(uint8_t d, uint8_t s) {
  uint8_t res = d ^ s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  return res;
}

void MulCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  uint16_t res = d * s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  uint8_t rl = Dst() & 0x06;  // low byte register
  uint8_t rh = rl | 0x01;     // high byte register
  cpu_->ActiveRegsBank()[rl] = res & 0xFF;
  cpu_->ActiveRegsBank()[rh] = (res >> 8) & 0xFF;
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
  res += RegNames[Dst()];
  return res;
}

string MemoryCmd::Params() {
  string suffix;
  if (AutoDec())
    suffix += "D";
  if (AutoInc())
    suffix += "I";
  if (Offs()) {
    suffix += " + ";
    suffix += to_string(Offs());  // offs may have value from -8 to +7 so I have to convert it to "int"
  }

  if (OpCode() == 0x90)  // LD
    return string(" ") + RegNames[Reg()] + string(", ") + PtrNames[Ptr()] + suffix;
  else if (OpCode() == 0xC0)  // ST
    return string(" ") + PtrNames[Ptr()] + suffix + string(", ") + RegNames[Reg()];
  else
    return "error";
}

void LoadFromMemoryCmd::Execute() {
  uint16_t ptr = cpu_->GetPair(Ptr()) + Offs();
  uint8_t val = cpu_->RAM[ptr];
  cpu_->ActiveRegsBank()[Reg()] = val;
  if (AutoInc())
    cpu_->IncPair(Reg());
}

void StoreToMemoryCmd::Execute() {
  uint8_t val = cpu_->ActiveRegsBank()[Reg()];
  uint16_t ptr = cpu_->GetPair(Ptr()) + Offs();
  cpu_->RAM[ptr] = val;
  if (AutoInc())
    cpu_->IncPair(Reg());
}

string PortCmd::Params() {
  if (OpCode() == 0xA0)
    return string(" ") + RegNames[Reg()] + string(", PINS") + to_string(Port());
  else if (OpCode() == 0xB0)
    return " PORT" + to_string(Port()) + ", " + RegNames[Reg()];
  else
    return "error";
}
