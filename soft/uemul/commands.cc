#include "commands.h"
#include "cpu.h"

using namespace std;

uint8_t ArithmCmd::DstVal() {
  return cpu_->ActiveRegsBank()[Dst()];
}

uint8_t ArithmCmd::SrcVal() {
  return IsConst() ? Const() : cpu_->ActiveRegsBank()[Src()];
}

string ArithmCmd::Params() {
  string rsrc = IsConst() ? to_string(cmd_ & 0xFF) : RegNames[Src()];
  return string(" ") + RegNames[Dst()] + string(", ") + rsrc;
}

void AddCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  uint8_t res = d + s;
  cpu_->Flags[flags::CF] = (d + s) > 255;
  cpu_->Flags[flags::ZF] = res == 0;
  cpu_->ActiveRegsBank()[Dst()] = res;
}

void AddcCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  bool cf = cpu_->Flags[flags::CF];
  uint8_t res = d + s + cf;
  cpu_->Flags[flags::CF] = (d + s + cf) > 255;
  cpu_->Flags[flags::ZF] = res == 0;
  cpu_->ActiveRegsBank()[Dst()] = res;
}

void AndCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  uint8_t res = d & s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  cpu_->ActiveRegsBank()[Dst()] = res;
}

void OrCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  uint8_t res = d | s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  cpu_->ActiveRegsBank()[Dst()] = res;
}

void XorCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  uint8_t res = d ^ s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  cpu_->ActiveRegsBank()[Dst()] = res;
}

void MulCmd::Execute() {
  uint8_t d = DstVal();
  uint8_t s = SrcVal();

  uint16_t res = d * s;
  cpu_->Flags[flags::CF] = false;
  cpu_->Flags[flags::ZF] = res == 0;
  uint8_t rl = Dst() & 0x06;
  uint8_t rh = rl |= 0x01;
  cpu_->ActiveRegsBank()[rl] = res & 0xFF;
  cpu_->ActiveRegsBank()[rh] = (res >> 8) & 0xFF;
}

void MovCmd::Execute() {
  cpu_->ActiveRegsBank()[Dst()] = SrcVal();
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

string PortCmd::Params() {
  if (OpCode() == 0xA0)
    return string(" ") + RegNames[Reg()] + string(", PINS") + to_string(Port());
  else if (OpCode() == 0xB0)
    return " PORT" + to_string(Port()) + ", " + RegNames[Reg()];
  else
    return "error";
}
