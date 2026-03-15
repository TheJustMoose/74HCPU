#pragma once

#include "data_type.h"
#include "operation.h"
#include "names.h"
#include "node.h"
#include "node_type.h"

#include <cctype>
#include <string>
#include <vector>

class Num: public Node {
 public:
  Num(int value);

  void gen(std::vector<Operation>& res_code) override;

 private:
  int value_ {0};
};

class Name: public Node {
 public:
  Name(std::string value);

  void init_size(uint8_t size);
  void gen(std::vector<Operation>& res_code) override;
  std::string tmp_name() override;
  std::string name();
  uint8_t cached_size();

 private:
  std::string value_ {""};
  uint8_t cached_size_ {0};
};

class BinOp: public Node {
 public:
  BinOp(Token t);

  void gen(std::vector<Operation>& res_code) override;
  std::string op() override;

  Node* left {nullptr};
  Node* right {nullptr};
};

class UnOp: public Node {
 public:
  UnOp();

  void gen(std::vector<Operation>& res_code) override;
  std::string op() override;

  Node* child {nullptr};
};

class AssignOp: public Node {
 public:
  AssignOp();

  void gen(std::vector<Operation>& res_code) override;
  std::string op() override;

  Node* left {nullptr};
  Node* right {nullptr};
};

class VarDecl: public Node {
 public:
  VarDecl(DataType dt, bool is_ptr);

  void gen(std::vector<Operation>& res_code) override;
  uint8_t var_size();

  DataType data_type {dtNotInitialize};
  bool is_pointer {false};
  std::vector<std::string> names {};
};
