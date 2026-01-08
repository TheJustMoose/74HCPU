///////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <clocale>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "assm.h"
#include "error_collector.h"
#include "file_reader.h"
#include "preprocessor.h"
#include "str_util.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

OP_TYPE CopToType(COP cop) {
  if (cop == cNO_OP)
    return tNO_OP;
  else if (cop >= cADD && cop <= cMUL)
    return tBINARY;
  else if (cop == cMOV || cop == cCMP || cop == cCMPC)
    return tBINARY;
  else if (cop == cUNO)
    return tUNARY;
  else if (cop == cLPM || cop == cLPMW || cop == cLD || cop == cLDV || cop == cST || cop == cSTV)
    return tMEMORY;
  else if (cop == cIN || cop == cOUT || cop == cTOGL)
    return tIO;
  else if (cop >= bCALL && cop <= bNOP)
    return tBRANCH;
  else  // I don't know when this should work
    return tNO_OP;
}

map<string, uint16_t> unary_codes {
  { "INV", 0x00 },   // 00
  { "SWAP", 0x20 },  // 01
  { "LSR", 0x40 },   // 10
  { "LSRC", 0x60 },  // 11
};

map<string, COP> cop_names {
  { "ADD", cADD}, { "ADDC", cADDC},
  { "AND", cAND}, { "OR", cOR}, { "XOR", cXOR},
  { "MUL", cMUL},
  { "INV", cUNO}, { "SWAP", cUNO}, { "LSR", cUNO}, { "LSRC", cUNO},
  { "MOV", cMOV},
  { "LPM", cLPM}, { "LPMW", cLPMW},
  { "LD", cLD}, { "LOAD", cLD}, { "LDV", cLDV},
  { "IN", cIN}, { "OUT", cOUT}, { "TOGL", cTOGL},
  { "ST", cST}, { "STORE", cST}, { "STV", cSTV},
  { "CMP", cCMP}, { "CMPC", cCMPC},
  { "CALL", bCALL},
  { "JMP", bJMP },
  { "RET", bRET },
  { "RETI", bRETI },
  { "JL", bJL },
  { "JE", bJE },
  { "JNE", bJNE },
  { "JG", bJG },
  { "JZ", bJZ },
  { "JNZ", bJNZ },
  { "JC", bJC },
  { "JNC", bJNC },
  { "JHC", bJHC },
  { "JNHC", bJNHC },
  { "AFCALL", bAFCALL },
  { "NOP", bNOP },
  { "STOP", bSTOP },
};

map<string, REG> reg_names {
  { "R0", rR0 }, { "R1", rR1 }, { "R2", rR2 }, { "R3", rR3 }, { "R4", rR4 }, { "R5", rR5 }, { "R6", rR6 }, { "R7", rR7 },
  { "XL", rXL}, { "XH", rXH}, { "YL", rYL}, { "YH", rYH}, { "VL", rVL}, { "VH", rVH}, { "SPL", rSPL}, { "SPH", rSPH},
};

map<string, PTR> ptr_names {
  { "X", rX }, { "Y", rY }, { "V", rV }, { "SP", rSP },
  { "XI", rXI }, { "YI", rYI }, { "VI", rVI }, { "SPI", rSPI },
  { "XD", rXD }, { "YD", rYD }, { "VD", rVD }, { "SPD", rSPD },
};

map<string, uint16_t> port_names {
  //  output          input          function
  { "PORT0", 0 }, { "PINS0", 0 },
  { "OMASK0", 1 },
  { "PORT1", 2 }, { "PINS1", 2 }, { "LCD_CTRL", 2 },
  { "OMASK0", 3 },
  { "PORT2", 4 }, { "LCD_DATA", 4 },
  { "PORT3", 5 }, { "PINS3", 5 }, { "RAMP", 5 },
  { "PORT4", 6 }, { "PINS4", 6 }, {"CPU_FLAGS", 6},
  { "PORT5", 7 }, { "PINS5", 7 }, { "MSW", 7 },
  { "PORT6", 8 }, { "PINS6", 8 }, { "DAC", 8 },
  { "PORT7", 9 }, { "PINS7", 9 },
  { "PORT8", 10 }, { "PINS8", 10 },
  { "PORT9", 11 }, { "PINS9", 11 },
};

///////////////////////////////////////////////////////////////////////////////

COP Names::CopFromName(string name) {
  if (name.empty())
    return bNOP;

  if (cop_names.find(name) != cop_names.end())
    return cop_names[name];
  else
    return bERROR;
}

REG Names::RegFromName(string name) {
  if (name.empty())
    return rUnkReg;

  if (reg_names.find(name) != reg_names.end())
    return reg_names[name];
  else
    return rUnkReg;
}

PTR Names::PtrFromName(string name, bool* inc = nullptr, bool* dec = nullptr) {
  if (name.empty())
    return rUnkPtr;

  size_t len = 0;
  while (len < name.size() && isalpha(name[len]))
    len++;
  if (len != name.size())
    name.resize(len);

  PTR ptr {rUnkPtr};
  if (ptr_names.find(name) != ptr_names.end()) {
    ptr = ptr_names[name];
    if (inc)
      *inc = ptr & rInc;
    if (dec)
      *dec = ptr & rDec;
    return (PTR)(ptr & rMask);
  } else {
    return rUnkPtr;
  }
}

int Names::PortFromName(string name, string prefix) {
  if (name.empty())
    return -1;

  if (port_names.find(name) == port_names.end())
    return -1;

  return port_names[name];
}

///////////////////////////////////////////////////////////////////////////////
class RightVal {
 public:
  RightVal() = default;

  void Init(int line_number, string right) {
    line_number_ = line_number;
    right_ = right;
    Parse();
  }

  void Parse() {
    if (right_.find("+1") != string::npos) {
      force_cf_ = true;
      right_.resize(right_.size() - 2);
    }

    // It's duplicate of StripPrefix but for immediate values:
    if (right_.find("LO(") == 0) {
      if (*right_.rbegin() != ')')
        ErrorCollector::GetInstance().err("Can't find right parentheses - )",
                                          line_number_);
      right_ = right_.substr(3, right_.size() - 4);
      lo_macro_ = true;
    } else if (right_.find("HI(") == 0) {
      if (*right_.rbegin() != ')')
        ErrorCollector::GetInstance().err("Can't find right parentheses - )",
                                          line_number_);
      right_ = right_.substr(3, right_.size() - 4);
      hi_macro_ = true;
    }

    int t = 0;
    immediate_ = StrToInt(right_, &t);  // MOV R1, 10
    if (immediate_) {
      if (t < 0)
        right_val_ = t & 0x00FF;  // TODO: how LO() should work when value is negative?
      else {
        if (lo_macro_)
          right_val_ = t & 0xFF;
        else if (hi_macro_)
          right_val_ = (t >> 8) & 0xFF;
        else
          right_val_ = t;
      }

      if (right_val_> 0xFF)
        ErrorCollector::GetInstance().err(
          "Error. Immediate value should have 8 bit only (0 - 255). Got: " + right_, line_number_);
    } else {  // not val, try to check register name
      string rest = StripPrefix(right_);
      right_reg_ = Names::RegFromName(rest);  // MOV R0, R1
      if (right_reg_ == rUnkReg) {
        ErrorCollector::GetInstance().err(
          "You have to use Register name or immediate value as rval. Now it's: " + rest, line_number_);
      }
    }
  }

  string StripPrefix(string val) {
    if (val.size() <= 2)  // strlen("R0") == 2
      return val;

    if (val[0] == '`' || val[1] == '`')
      inv_hi_ = true;
    if (val[0] == '\'' || val[1] == '\'')
      inv_lo_ = true;

    bool inv_all {false};
    if (val[0] == '~' || val[1] == '~')
      inv_all = true;

    int cnt = inv_lo_ + inv_hi_ + inv_all;
    if (cnt > 2)
      cnt = 2;
    if (cnt)
      val = val.substr(cnt, val.size() - cnt);

    if (inv_all) {
      inv_hi_ = true;
      inv_lo_ = true;
    }

    if (val.find("L(") == 0 && *val.rbegin() == ')') {
      zero_hi_ = true;
      return val.substr(2, val.size() - 3);
    }
    if (val.find("H(") == 0 && *val.rbegin() == ')') {
      zero_lo_ = true;
      return val.substr(2, val.size() - 3);
    }
    return val;
  }

  bool immediate() { return immediate_; }
  uint16_t val() { return right_val_; }
  REG reg() { return right_reg_; }
  int LineNumber() { return line_number_; }
  string str_val() { return right_; }

  bool zero_lo() { return zero_lo_; }
  bool zero_hi() { return zero_hi_; }
  bool inv_lo() { return inv_lo_; }
  bool inv_hi() { return inv_hi_; }
  bool force_cf() { return force_cf_; }
  bool lo_macro() { return lo_macro_; }
  bool hi_macro() { return hi_macro_; }

  void update_val(uint16_t val) {
    right_val_ = val;
    immediate_ = true;
  }

 private:
  bool immediate_ {false};
  uint16_t right_val_ {0};
  REG right_reg_ {rUnkReg};
  int line_number_ {0};
  bool zero_lo_ {false};
  bool zero_hi_ {false};
  bool inv_lo_ {false};
  bool inv_hi_ {false};
  bool force_cf_ {false};
  bool lo_macro_ {false};
  bool hi_macro_ {false};
  string right_;
};
///////////////////////////////////////////////////////////////////////////////

// ADD R0, R1
// ADD R2, 10
class BinaryCodeGen: public CodeGen {
 public:
  BinaryCodeGen(int line_number, COP cop, string left, string right)
    : CodeGen(line_number, cop) {
    left_op_ = Names::RegFromName(left);
    if (left_op_ == rUnkReg)
      ErrorCollector::GetInstance().err("Unknown register: " + left, line_number);

    right_val_.Init(line_number, right);
  }

  uint16_t Emit() {
    uint16_t cop = operation_;
    cop |= left_op_ << 9;
    if (right_val_.immediate()) {  // don't forget about C bit
      cop |= right_val_.val();
      cop |= 0x0100;  // set C bit to 1
    }
    else {
      cop |= right_val_.reg() << 5;
      if (right_val_.inv_lo())
        cop |= cINV_LO;
      if (right_val_.inv_hi())
        cop |= cINV_HI;
      if (right_val_.zero_lo())
        cop |= cZERO_LO;
      if (right_val_.zero_hi())
        cop |= cZERO_HI;
      if (right_val_.force_cf())
        cop |= cFORCE_CF;
    }
    return cop;
  }

  void UpdateMachineCode(const map<string, uint16_t>& name_to_address) {
    if (right_val_.str_val().empty())
      return;

    map<string, uint16_t>::const_iterator it;
    for (it = name_to_address.begin(); it != name_to_address.end(); it++) {
      string t = ToUpper(it->first);
      if (t == right_val_.str_val())
        break;
    }

    if (it == name_to_address.end())
      return;

    ErrorCollector::GetInstance().clr(LineNumber());  // have to remove previous messages
                                         // cause RegFromName know nothing about labels and LO/HI macro
    uint16_t rv = it->second;
    if (right_val_.hi_macro()) {
      rv >>= 8;
      rv &= 0x00FF;
    } else if (right_val_.lo_macro()) {
      rv &= 0x00FF;
    }
    right_val_.update_val(rv);

    ErrorCollector::GetInstance().rep(
      "Replace " + it->first + " to " + to_string(rv), LineNumber());
  }

  vector<int> GetBlocks() {
    if (right_val_.immediate())
      //    COP dst C value
      return {4, 3, 1, 8};
    else
      //    COP dst C src F
      return {4, 3, 1, 3, 1};
  }

 private:
  REG left_op_ {rUnkReg};
  RightVal right_val_ {};
};

// LSR R7
class UnaryCodeGen: public CodeGen {
 public:
  UnaryCodeGen(int line_number, string op_name, string reg)
    : CodeGen(line_number, cUNO), op_name_(op_name) {
    reg_ = Names::RegFromName(reg);
    if (reg_ == rUnkReg)
      ErrorCollector::GetInstance().err("Unknown register: " + reg, line_number);
    if (unary_codes.find(op_name) != unary_codes.end())
      ucode_ = unary_codes[op_name];
    else {
      ucode_ = 0;
      ErrorCollector::GetInstance().err("Unknown unary operation ", line_number);
    }
  }

  uint16_t Emit() {
    uint16_t cop = operation_;
    cop |= ucode_;
    cop |= reg_ << 9;  // don't forget about C bit
    return cop;
  }

  vector<int> GetBlocks() {
    //    COP reg 0  - TYP F
    return {4, 3, 1, 1, 2, 1};
  }

 private:
  string op_name_;
  uint16_t ucode_ {0};
  REG reg_ {rUnkReg};
};

// LD R0, X
// LD R1, XI + 10
class MemoryCodeGen: public CodeGen {
 public:
  MemoryCodeGen(int line_number, COP cop, string left, string right)
    : CodeGen(line_number, cop) {
    if (cop == cLD || cop == cLDV || cop == cLPM || cop == cLPMW) {
      reg_ = Names::RegFromName(left);
      if (reg_ == rUnkReg)
        ErrorCollector::GetInstance().err("Unknown register: " + left, line_number);
      ptr_ = Names::PtrFromName(right, &inc_, &dec_);
      if (ptr_ == rUnkPtr)
        ErrorCollector::GetInstance().err("Unknown pointer register: " + right, line_number);
      offset_ = parse_offset(right);
    } else if (cop == cST || cop == cSTV) {
      ptr_ = Names::PtrFromName(left, &inc_, &dec_);
      if (ptr_ == rUnkPtr)
        ErrorCollector::GetInstance().err("Unknown pointer register: " + left, line_number);
      reg_ = Names::RegFromName(right);
      if (reg_ == rUnkReg)
        ErrorCollector::GetInstance().err("Unknown register: " + right, line_number);
      offset_ = parse_offset(left);
    } else {
      ErrorCollector::GetInstance().err("Unknown memory operation. Should be LD/LDV or ST/STV or LPM.", line_number);
    }
  }

  uint16_t parse_offset(string tail) {
    if (tail.empty())
      return 0;

    while (tail.size() && isalpha(*tail.begin()))
      tail = tail.erase(0, 1);

    if (tail.empty())
      return 0;

    if (tail[0] != '+' && tail[0] != '-') {
      ErrorCollector::GetInstance().err(
        "You can add or substract offset from pointer register. No other operations.",
        LineNumber());
      return 0;
    }

    if (tail.size() < 2) {
      ErrorCollector::GetInstance().err("You lost offset value.", LineNumber());
      return 0;
    }

    uint16_t res {0};
    int off = stoi(&tail[1], nullptr, 10);
    if (tail[0] == '-')
      off = -off;
    if (off > 7)
      ErrorCollector::GetInstance().err("Offset has to be <= 7.", LineNumber());
    else if (off < -8)
      ErrorCollector::GetInstance().err("Offset has to be >= -8.", LineNumber());
    else
      res = off & 0x0F;  // we need only 4 bits
    return res;
  }

  uint16_t Emit() {
    uint16_t cop = operation_;  // COP already contain V flag
    cop |= reg_ << 9;
    cop |= ptr_ << 6;
    cop |= dec_ << 5;
    cop |= inc_ << 4;
    cop |= offset_;
    return cop;
  }

  vector<int> GetBlocks() {
    //    COP dst V EXT D  U  OFFSET
    return {4, 3, 1, 2, 1, 1, 4};
  }

 private:
  REG reg_ {rUnkReg};
  PTR ptr_ {rUnkPtr};
  bool inc_ {false};  // post increment ptr_
  bool dec_ {false};  // post decrement ptr_
  uint16_t offset_ {0};
};

// IN R0, PINS1
class InputCodeGen: public CodeGen {
 public:
  InputCodeGen(int line_number, COP cop, string left, string right)
    : CodeGen(line_number, cop) {
    reg_ = Names::RegFromName(left);
    if (reg_ == rUnkReg)
      ErrorCollector::GetInstance().err("Unknown register: " + left, line_number);

    int p = Names::PortFromName(right, "PIN");
    if (p == -1)
      ErrorCollector::GetInstance().err("Unknown port name: " + right, line_number);
    else if (p >= 32)
      ErrorCollector::GetInstance().err("Port number should be in range 0-31", line_number);
    else
      port_ = p;
  }

  uint16_t Emit() {
    uint16_t cop = operation_;
    //      F E D C  B A 9 8 7 6 5 4 3 2 1 0
    //| A0 |    IN |  DST |  PORT   |Z|z|I|i|
    cop |= reg_ << 9;
    cop |= port_ << 4;
    return cop;
  }

  vector<int> GetBlocks() {
    //   COP dst port -- Ii
    return {4, 3, 5, 2, 2};
  }

 private:
  uint16_t port_ {0};
  REG reg_ {rUnkReg};
};

// OUT PORT2, R7
// OUT PORT3, 10
class OutputCodeGen: public CodeGen {
 public:
  OutputCodeGen(int line_number, COP cop, string left, string right)
    : CodeGen(line_number, cop) {

    RightVal rv;
    rv.Init(line_number, right);
    immediate_ = rv.immediate();
    reg_ = rv.reg();
    right_val_ = rv.val();

    if (immediate_ && cop == cTOGL)
      ErrorCollector::GetInstance().err("Sorry. You can't use TOGL with immediate value", line_number);

    int p = Names::PortFromName(left, "PORT");
    if (p == -1)
      ErrorCollector::GetInstance().err("Unknown port name: " + left, line_number);
    else if (immediate_ && p >= 8)
      ErrorCollector::GetInstance().err("Port number with immediate value should be in range 0-7", line_number);
    else if (p >= 32)
      ErrorCollector::GetInstance().err("Port number should be in range 0-31", line_number);
    else
      port_ = p;
  }

  uint16_t Emit() {
    uint16_t cop = operation_;  // cTOGL already has low bits
    //      F E D C  B A 9 8 7 6 5 4 3 2 1 0
    //|              2 1 0         4 3
    //| B0 |   OUT | PORT |0| SRC |PRT|X|O|o|
    //| B0 |   OUT | PORT |1|     CONST     |
    cop |= (port_ & 0x07) << 9;
    if (immediate_) {
      cop |= 0x0100;  // set C bit to 1
      cop |= (right_val_ & 0x0FF);
    } else {
      cop |= reg_ << 5;
      cop |= (port_ & 0x18) << 3;  // 2 high bits of port
    }
    return cop;
  }

  vector<int> GetBlocks() {
    if (immediate_)
      //   COP port C const
      return {4, 3, 1, 8};
    else
      //   COP port C src prt XOo
      return {4, 3, 1, 3, 2, 3};
  }

 private:
  uint16_t port_ {0};
  REG reg_ {rUnkReg};
  uint16_t right_val_ {0};
  bool immediate_ {false};
};

class BranchCodeGen: public CodeGen {
 public:
  BranchCodeGen(int line_number, COP cop, string label)
    : CodeGen(line_number, cop), label_(label) {}

  uint16_t Emit() {
    uint16_t cop = operation_;
    cop |= (uint16_t)target_addr_;
    if (operation_ == bNOP)
      cop |= 0xFF;
    return cop;
  }

  void UpdateMachineCode(const map<string, uint16_t>& name_to_address) {
    map<string, uint16_t>::const_iterator it;
    for (it = name_to_address.begin(); it != name_to_address.end(); it++) {
      if (ToUpper(it->first) == label_) {
        uint16_t label_addr = it->second;
        if (operation_ == bAFCALL) {
          if (label_addr % 256) {
            ErrorCollector::GetInstance().err(
                "Label address in far call must be a multiple of 256. Now addr: "
                + to_string(label_addr), LineNumber());
          }
          target_addr_ = label_addr >> 8;  // absolute address divided by 256
        } else {
          int offset = label_addr;
          offset -= address_;         // offset from current address
          if (offset > 127 || offset < -128)
            ErrorCollector::GetInstance().err(
                "Error: Label " + label_ + " is too far from this instruction: "
                + to_string(offset), LineNumber());
          else
            target_addr_ = offset & 0xFF;
        }
      }
    }
  }

  vector<int> GetBlocks() {
    //    COP TYP offset/addr
    return {4, 4, 8};
  }

 private:
  string label_;
  uint16_t target_addr_ {0};
};

///////////////////////////////////////////////////////////////////////////////

CodeLine::CodeLine(int line_number, string line_text)
  : line_number_(line_number), line_text_(line_text) {

  size_t pos = line_text_.find(":");
  while (pos != string::npos) {
    string label = line_text_.substr(0, pos);
    if (!label.empty()) {
      labels_.push_back(label);
    }
    line_text_.erase(0, pos + 1);
    pos = line_text_.find(":");
  }

  vector<string> cmd_parts = SplitToCmdParts(line_text_);
  if (cmd_parts.size() != 3) {
    cout << "Error. SplitToCmdParts should always return 3 items.";
    return;
  }

  string op_name( cmd_parts[0] );
  string left( cmd_parts[1] );
  string right( cmd_parts[2] );

  //cout << "GOT: |" << op_name << "|" << left << "|" << right << "|" << endl;

  // Try to convert SUB to ADDC cmd:
  if (op_name == "SUB") {
    REG r = Names::RegFromName(right);
    if (r == rUnkReg) {
      ErrorCollector::GetInstance().err(
          "Sorry. You can use only register in right part of SUB cmd: " + right +
          ". Try to replace SUB to ADD", line_number);
      return;
    }

    op_name = "ADDC";
    right = "~" + right + "+1";
  }

  COP op = Names::CopFromName(op_name);
  if (op == bERROR) {
    ErrorCollector::GetInstance().err("Unknown operation: " + op_name, line_number);
    return;
  }

  OP_TYPE opt = CopToType(op);

  CodeGen* cg {nullptr};
  switch (opt) {
    case tBINARY: cg = new BinaryCodeGen(line_number_, op, left, right); break;
    case tUNARY: cg = new UnaryCodeGen(line_number_, op_name, left); break;
    case tMEMORY: cg = new MemoryCodeGen(line_number_, op, left, right); break;
    case tIO: {
      if (op == cIN)
        cg = new InputCodeGen(line_number_, op, left, right);
      else
        cg = new OutputCodeGen(line_number_, op, left, right);
    } break;
    case tBRANCH: cg = new BranchCodeGen(line_number_, op, left); break;
    case tNO_OP:
    default:
      cout << "No operation is required" << endl;
  }

  code_gen_.reset(cg);
}

uint16_t CodeLine::GenerateMachineCode() {
  if (!code_gen_)
    return bNOP | 0xFF;

  return code_gen_->Emit();
}

void CodeLine::UpdateMachineCode(const map<string, uint16_t>& name_to_address) {
  if (!code_gen_)
    return;
  code_gen_->UpdateMachineCode(name_to_address);
}

string CodeLine::FormattedCOP() {
  if (!code_gen_)
    return {};
  return code_gen_->FormattedCOP();
}

///////////////////////////////////////////////////////////////////////////////

StringConst& StringConst::operator=(const StringConst& rval) {
  this->str_ = rval.str_;
  this->line_number_ = rval.line_number_;
  this->address_ = rval.address_;
  return *this;
}

uint16_t StringConst::GetSize() const {
  if (str_.size() >= numeric_limits<uint16_t>::max()) {
    ErrorCollector::GetInstance().err("String const is too long.", line_number_);
    return 0;
  }

  return static_cast<uint16_t>(str_.size()) + 1;
}

void StringConst::SetAddress(uint16_t address) {
  if (address > 0xFFFF) {
    cout << "Address is too much: " << address;
    return;
  }

  address_ = address;
}

void StringConst::OutCode() const {
  uint16_t address = address_;
  for (size_t i = 0; i < str_.size(); i++)
    cout << "     " << hex
       << setw(4) << setfill('0') << right << address++ << ": "
       << setw(4) << setfill('0') << right << uint16_t(str_[i]) << " "
       << str_[i] << endl;

  cout << "     " << hex
       << setw(4) << setfill('0') << right << address++ << ": "
       << setw(4) << setfill('0') << right << uint16_t(0) << " "
       << "Zero" << endl;
}

void StringConst::OutCode(vector<uint16_t>& code) const {
  if (!str_.size())
    return;

  // string with str_.size() == 1 will store char at address_
  // and trailing zero at address address_ + 1, so:
  uint16_t max_str_address = address_ + static_cast<uint16_t>(str_.size());

  if (max_str_address + 1 > code.size())
    code.resize(max_str_address + 1, 0xFFFFU);

  for (size_t i = 0; i < str_.size(); i++)
    code[address_ + i] = str_[i];
  code[max_str_address] = 0;
}

///////////////////////////////////////////////////////////////////////////////

int Assembler::Process(string fname, bool show_preprocess_out) {
  FileReader fr;
  int res = fr.read_file(fname, &lines_);
  if (res != 0)
    return res;

  Preprocessor pre;
  pre.Preprocess(&lines_);

  if (show_preprocess_out)
    PrintPreprocessed();

  MergeCodeWithLabels();
  ExtractOrgs();
  ExtractString();
  Pass1();
  Pass2();
  Pass3();
  OutCode();

  OutLabels();
  OutOrgs();

  return ErrorCollector::GetInstance().have_errors();
}

void Assembler::PrintPreprocessed() {
  cout << "-------- preprocessed source: --------" << endl;
  map<int, string>::iterator it;
  for (it = lines_.begin(); it != lines_.end(); it++)
    cout << it->second << endl;
  cout << "-------- end of preprocessed source --------" << endl;
}

void Assembler::MergeCodeWithLabels() {
  map<int, string>::iterator it, prev;
  for (it = lines_.begin(); it != lines_.end();) {
    prev = it;
    it++;
    if (*prev->second.rbegin() == ':' &&       // ok, label was found
        it != lines_.end()) {                  // and we have some code on the next line
      it->second = prev->second + it->second;  // move label to the next line
      lines_.erase(prev);                      // remove label from curent line
    }
  }
}

void Assembler::ExtractOrgs() {
  map<int, string>::iterator it;
  for (it = lines_.begin(); it != lines_.end();) {
    int line = it->first;
    string org = NormalizeLine(it->second);
    if (org.find(".org") == 0) {
      cout << line << ":" << org << endl;
      org.erase(0, sizeof(".org"));

      int val {0};
      string msg_err;
      if (StrToInt(ToUpper(org), &val, &msg_err)) {
        line_to_org_[line] = val;
        cout << "now line " << line << " has address " << val << endl;
      } else
        cout << "Can't convert org value to int" << endl << msg_err << endl;
      it = lines_.erase(it);  // now remove this directive from asm
    }
    else
      it++;
  }
}

void Assembler::ExtractString() {
  map<int, string>::iterator it;
  for (it = lines_.begin(); it != lines_.end();) {
    string str = NormalizeLine(it->second);
    if (str.find(".str") == 0) {
      str.erase(0, sizeof(".str"));
      size_t pos = str.find(" ");
      if (pos == string::npos) {
        cout << "Error in string const: '" << it->second << "'" << endl;
        cout << "Line: " << it->first << ". String value not found" << endl;
      } else if (pos < str.size()) {
        string str_name = Trim(str.substr(0, pos));
        string str_val = RemoveQuotes(Trim(str.substr(pos + 1)));
        string_consts_[str_name] = StringConst(str_val, it->first);
        cout << "STR '" << str_name << "' = '" << str_val << "'" << endl;
      }
      it = lines_.erase(it);  // now remove this directive from asm
    }
    else
      it++;
  }
}

// generate machine code
void Assembler::Pass1() {
  map<int, string>::iterator it;
  for (it = lines_.begin(); it != lines_.end(); it++) {
    string nl = NormalizeLine(it->second);
    if (nl.size() == 0)
      continue;
    code_.push_back(CodeLine(it->first, nl));
  }
}

// get real address of labels & string
void Assembler::Pass2() {
  uint16_t addr = 0;
  vector<CodeLine>::iterator it;
  map<int, uint16_t>::iterator oit = line_to_org_.begin();
  // for each line of code
  for (it = code_.begin(); it != code_.end(); it++, addr++) {
    if (oit != line_to_org_.end() &&
        it->LineNumber() > oit->first) {  // if line placed after .org
      if (oit->second > addr) {
        addr = oit->second;                // change line address to .org value
        cout << "move address to: " << addr << endl;
      }
      oit++;  // this .org was used, we need new .org
    }

    it->SetAddress(addr);  // change address of every line
    // and every label of this line
    vector<string> labels = it->GetLabels();
    vector<string>::iterator lit;
    for (lit = labels.begin(); lit != labels.end(); lit++)
      name_to_address_[*lit] = addr;
  }

  uint16_t max_addr = GetMaxAddress();
  cout << "max_addr: " << max_addr << " (" << hex << max_addr << "h)" << endl;

  // now place strings after binary code
  addr = max_addr + 1;
  for (auto& s : string_consts_) {
    if (oit != line_to_org_.end() &&
        s.second.LineNumber() > oit->first) {  // if string is placed after .org
      addr = oit->second;                // we have to change its addresses
      oit++;  // probably we have more .orgs!
    }
    s.second.SetAddress(addr);
    name_to_address_[s.first] = addr;
    addr += s.second.GetSize();
  }
}

// set real jump addresses
void Assembler::Pass3() {
  vector<CodeLine>::iterator it;
  for (it = code_.begin(); it != code_.end(); it++)
    it->UpdateMachineCode(name_to_address_);
}

uint16_t Assembler::GetMaxAddress() {
  uint16_t max_addr {0};
  vector<CodeLine>::iterator it;
  for (it = code_.begin(); it != code_.end(); it++)
    if (max_addr < it->Address())
      max_addr = it->Address();
  return max_addr;
}

void Assembler::OutCode() {
  cout << "STRINGS ADDR:" << endl;
  for (const auto& s : string_consts_)
    cout << s.first << ": " << s.second.Address() << endl;

  cout << "LINE ADDR: COP   ASM                       LABELS          FORMATTED_COP" << endl;
  vector<CodeLine>::iterator it;
  for (it = code_.begin(); it != code_.end(); it++) {
    cout << dec
         << setw(4) << setfill(' ') << right << it->LineNumber() << " "
         << hex
         << setw(4) << setfill('0') << right << it->Address() << ": "
         << setw(4) << setfill('0') << right << it->GenerateMachineCode() << "  "
         << setw(24) << setfill(' ') << left << it->GetLineText() << "  "
         << setw(16) << setfill(' ') << left << Join(it->GetLabels(), ' ')
         << setw(16) << setfill(' ') << left << it->FormattedCOP()
         << endl;

    string el = ErrorCollector::GetInstance().get(it->LineNumber());
    if (el.size())  // for (auto& e : el)
      cout << "         > " << el << endl;
  }

  cout << "STRINGS:" << endl;
  if (string_consts_.empty())
    cout << " empty" << endl;
  for (auto& s : string_consts_) {
    cout << s.first << endl;
    s.second.Address();
    s.second.OutCode();
  }
}

void Assembler::OutCode(vector<uint16_t>& code) {
  code.clear();

  uint16_t max_addr = GetMaxAddress();
  if (!max_addr) {
    cout << "No code to output" << endl;
    return;
  }

  // numbers from 0 to 5 have 6 pcs (max + 1)
  code.resize(max_addr + 1, 0xFFFFU);

  vector<CodeLine>::iterator it;
  for (it = code_.begin(); it != code_.end(); it++)
    code[it->Address()] = it->GenerateMachineCode();

  // StringConst::out_code will resize 'code' if required
  for (auto& s : string_consts_)
    s.second.OutCode(code);

  if (name_to_address_.empty())
    return;

  // try to add some 'debug' info into the end of file
  code.push_back(0);
  code.push_back(0);
  code.push_back(0);
  code.push_back(0);
  code.push_back('D');  // just label to identify this block
  code.push_back('B');
  code.push_back('G');
  code.push_back('I');
  for (auto v : name_to_address_) {
    code.push_back(v.second);  // label address
    string lbl = v.first;  // label name
    for (size_t i = 0; i < lbl.size(); i++)
      code.push_back(lbl[i]);
    code.push_back(0);  // ASCIIZ string
  }
}

void Assembler::WriteBinary(string fname) {
  vector<uint16_t> code;
  OutCode(code);
  if (!code.size())
    return;

  ofstream f;
  f.open(fname, ios::binary);
  if (!f) {
    cout << "Error. Can't open file " << fname << endl;
    return;
  }

  f.write(reinterpret_cast<const char*>(code.data()),
          code.size()*sizeof(uint16_t));
  f.close();
}

void Assembler::OutLabels() {
  cout << "LABELS:" << endl;
  if (name_to_address_.empty())
    cout << " empty" << endl;
  for (auto v : name_to_address_)
    //       label             address
    cout << v.first << " " << v.second << endl;
}

void Assembler::OutOrgs() {
  cout << "ORGS:" << endl;
  if (line_to_org_.empty())
    cout << " empty" << endl;
  for (auto v : line_to_org_)
    cout << v.first << " " << v.second << endl;
}

///////////////////////////////////////////////////////////////////////////////
