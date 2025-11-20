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
