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
  const char** names = cpu_->ActiveRegsNames();
  string rsrc = IsConst() ? to_string(cmd_ & 0xFF) : names[Src()];
  return string(" ") + names[Dst()] + string(", ") + rsrc;
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

string MemoryCmd::Suffix() {
  string suffix;
  if (AutoDec())
    suffix += "D";
  if (AutoInc())
    suffix += "I";
  if (Offs()) {
    suffix += " + ";
    suffix += to_string(Offs());  // offs may have value from -8 to +7 so I have to convert it to "int"
  }
  return suffix;
}

string LoadFromMemoryCmd::Params() {
  return string(" ") + cpu_->ActiveRegsNames()[Reg()] + string(", ") + PtrNames[Ptr()] + Suffix();
}

void LoadFromMemoryCmd::Execute() {
  uint16_t ptr = cpu_->GetPair(Ptr()) + Offs();
  uint8_t val = cpu_->RAM[ptr];
  cpu_->ActiveRegsBank()[Reg()] = val;
  if (AutoInc())
    cpu_->IncPair(Ptr());
  if (AutoDec())
    cpu_->DecPair(Ptr());
}

string StoreToMemoryCmd::Params() {
  return string(" ") + PtrNames[Ptr()] + Suffix() + string(", ") + cpu_->ActiveRegsNames()[Reg()];
}

void StoreToMemoryCmd::Execute() {
  uint8_t val = cpu_->ActiveRegsBank()[Reg()];
  uint16_t ptr = cpu_->GetPair(Ptr()) + Offs();
  cpu_->RAM[ptr] = val;
  if (AutoInc())
    cpu_->IncPair(Ptr());
  if (AutoDec())
    cpu_->DecPair(Ptr());
}

void LpmCmd::Execute() {
  uint16_t ptr = cpu_->GetPair(Ptr());
  uint16_t val = ptr < cpu_->ROM.size() ? cpu_->ROM[ptr] : 0xFFFF;
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
  uint8_t rdst = cpu_->PINS[Port()];
  cpu_->ActiveRegsBank()[Reg()] = rdst;
}

string OutputPortCmd::Params() {
  return " PORT" + to_string(Port()) + ", " + cpu_->ActiveRegsNames()[Reg()];
}

void OutputPortCmd::Execute() {
  cpu_->PORTS[Port()] = cpu_->ActiveRegsBank()[Reg()];
}
