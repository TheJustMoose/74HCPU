#include "commands.h"
#include "cpu.h"

using namespace std;

uint8_t ArithmCmd::dst_val() {
  return cpu_->ActiveRegsBank()[dst()];
}

string ArithmCmd::Params() {
  string rsrc = is_cnst() ? to_string(cmd_ & 0xFF) : RegNames[src()];
  return string(" ") + RegNames[dst()] + string(", ") + rsrc;
}

uint8_t UnaryCmd::dst_val() {
  return cpu_->ActiveRegsBank()[dst()];
}

string UnaryCmd::Params() {
  string res = UnoNames[type()];
  res += " ";
  res += RegNames[dst()];
  return res;
}

string MemoryCmd::Params() {
  string suffix;
  if (autodec())
    suffix += "D";
  if (autoinc())
    suffix += "I";
  if (offs()) {
    suffix += " + ";
    suffix += to_string(offs());  // offs may have value from -8 to +7 so I have to convert it to "int"
  }

  if (op() == 0x90)  // LD
    return string(" ") + RegNames[reg()] + string(", ") + PtrNames[ptr()] + suffix;
  else if (op() == 0xC0)  // ST
    return string(" ") + PtrNames[ptr()] + suffix + string(", ") + RegNames[reg()];
  else
    return "error";
}

string PortCmd::Params() {
  if (op() == 0xA0)
    return string(" ") + RegNames[reg()] + string(", PINS") + to_string(port());
  else if (op() == 0xB0)
    return " PORT" + to_string(port()) + ", " + RegNames[reg()];
  else
    return "error";
}
