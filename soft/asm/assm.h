#pragma once

#include <bitset>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

// Code Of Operation
enum COP {
  cADD = 0x0000, cADDC = 0x1000,
  cAND = 0x2000, cOR = 0x3000, cXOR = 0x4000,
  cMUL = 0x5000, cUNO = 0x6000, cMOV = 0x7000,
  cLPM = 0x8000, cLPMW = 0x8100, cLD = 0x9000,
  cIN = 0xA000, cOUT = 0xB000, cTOGL = 0xB004,
  cST = 0xC000,
  cCMP = 0xD000, cCMPC = 0xE000,
  bCALL = 0xF000,
  bJMP = 0xF100,
  bRET = 0xF200,
  bRETI = 0xF300,
  bJL = 0xF400,
  bJE = 0xF500,
  bJNE = 0xF600,
  bJG = 0xF700,
  bJZ = 0xF800,
  bJNZ = 0xF900,
  bJC = 0xFA00,
  bJNC = 0xFB00,
  bJHC = 0xFC00,
  bJNHC = 0xFD00,
  bAFCALL = 0xFE00,
  bNOP = 0xFFFF,
  bSTOP = 0xFFFE,  // processor operation
  bERROR = 0x10000,  // we get some unknown name of operation
  cNO_OP = 0x20000  // just constant for "there is no operation here", for example for labels or error op names
};

enum OP_TYPE {
  tBINARY, tUNARY, tMEMORY, tIO, tBRANCH, tNO_OP
};

enum REG : uint16_t {
  // Arithmetic registers
  rR0 = 0, rR1 = 1, rR2 = 2, rR3 = 3, rR4 = 4, rR5 = 5, rR6 = 6, rR7 = 7,   // Bank 0
  // Pointer registers
  rXL = 0, rXH = 1, rYL = 2, rYH = 3, rZL = 4, rZH = 5, rSPL = 6, rSPH = 7, // Bank 1
  rUnkReg = 0x100
};

enum PTR : uint16_t {
  // Pointer register pairs
  rX = 0, rY = 1, rZ = 2, rSP = 3,
  // Pointer register mask
  rMask = 0x3,
  // Flags
  rInc = 0x10, rDec = 0x20,
  // Same pointers but with post increment
  rXI = rX | rInc, rYI = rY | rInc, rZI = rZ | rInc, rSPI = rSP | rInc,
  // Same pointers but with post decrement
  rXD = rX | rDec, rYD = rY | rDec, rZD = rZ | rDec, rSPD = rSP | rDec,
  rUnkPtr = 0x100
};

class Names {
 public:
  static COP CopFromName(std::string name);
  static REG RegFromName(std::string name);
  static PTR PtrFromName(std::string name, bool* inc, bool* dec);
  static int PortFromName(std::string name, std::string prefix);
};

class CodeGen {
 public:
  CodeGen(int line_number, COP cop)
   : line_number_(line_number), operation_(cop) {}
  virtual ~CodeGen() {}

  virtual uint16_t Emit() { return 0; }
  virtual void update_machine_code(const std::map<std::string, uint16_t>& name_to_address) {}

  virtual std::vector<int> get_blocks() { return {}; }
  virtual std::string cop() {
    std::stringstream s;
    s << std::bitset<16>(Emit());
    return s.str();
  }
  std::string FormattedCOP() {
    std::string res = cop();
    std::vector<int> blocks = get_blocks();
    if (blocks.empty())
      return res;

    size_t pos {0};
    for (auto b : blocks) {
      pos += b;
      if (pos >= res.size())
        break;
      res.insert(pos, " ");
      pos++;
    }
    return res;
  }
  uint16_t address() {
    return address_;
  }
  void set_address(uint16_t addr) {
    address_ = addr;
  }

  int line_number() {
    return line_number_;
  }

 protected:
  int line_number_ {0};
  uint16_t address_ {0};
  COP operation_ {cNO_OP};
};

class CodeLine {
 public:
  CodeLine(int line_number, std::string line_text);

  uint16_t generate_machine_code();
  void update_machine_code(const std::map<std::string, uint16_t>& name_to_address);
  std::string FormattedCOP();

  std::vector<std::string> get_labels() {
    return labels_;
  }

  std::string get_labels_as_string() {
    if (!has_labels())
      return {};

    std::string res;
    for (auto& l : labels_) {
      res += l;
      res += " ";
    }
    return res;
  }

  bool has_labels() {
    return labels_.size() > 0;
  }

  std::string get_line_text() {
    return line_text_;
  }

  uint16_t address() {
    return code_gen_ ? code_gen_->address() : 0;
  }

  void set_address(uint16_t addr) {
    if (code_gen_)
      code_gen_->set_address(addr);
  }

  int line_number() {
    return line_number_;
  }

 private:
  int line_number_ {0};

  std::unique_ptr<CodeGen> code_gen_ {nullptr};

  std::vector<std::string> labels_ {};
  std::string line_text_ {};
};

class StringConst {
 public:
  StringConst() = default;
  StringConst(const std::string& str, int line): str_(str), line_(line) {}
  StringConst& operator=(const StringConst& rval);

  uint16_t get_size() const;
  int line() const { return line_; }
  uint16_t addr() const { return addr_; }
  void set_addr(uint16_t);
  void out_code() const;
  void out_code(std::vector<uint16_t>& code) const;
  std::string str() const { return str_; }

 private:
  std::string str_ {};
  int line_ {0};
  uint16_t addr_ {0};
};

class Assembler {
 public:
  int process(std::string fname, bool show_preprocess_out = false);
  void write_binary(std::string fname);

 protected:
  void print_preprocessed();
  void merge_code_with_labels();
  void extract_orgs();
  void extract_string();
  void extract_defs();
  void pass1();
  void pass2();
  void pass3();
  uint16_t get_max_address();
  void out_code();
  void out_code(std::vector<uint16_t>& code);
  void out_labels();
  void out_orgs();

 private:
  std::map<int, std::string> lines_ {};
  std::map<int, uint16_t> line_to_org_ {};
  std::vector<CodeLine> code_ {};
  std::map<std::string, uint16_t> name_to_address_ {};
  std::map<std::string, StringConst> string_consts_ {};
};
