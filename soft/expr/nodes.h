#pragma once

#include "data_type.h"
#include "operation.h"
#include "names.h"
#include "node.h"
#include "node_type.h"

#include <cstdint>
#include <string>
#include <vector>

class Num: public Node {
 public:
  Num(int value);

  void gen(std::vector<Operation>& res_code) override;
  std::string name() const override {
    return std::to_string(value_);
  }
  int value() const { return value_; }

 private:
  int value_ {0};
};

class Name: public Node {
 public:
  Name(std::string value);

  void init_size(uint8_t size);
  void gen(std::vector<Operation>& res_code) override;
  std::string name() const override;
  uint8_t cached_size();

 private:
  std::string value_ {""};
  uint8_t cached_size_ {0};
};

class BinOp: public Node {
 public:
  BinOp(Token t, std::string n);

  void gen(std::vector<Operation>& res_code) override;
  std::string name() const override {
    return name_;
  }
  std::string op() const override;

  Node* left {nullptr};
  Node* right {nullptr};

 private:
  std::string name_;
};

class UnOp: public Node {
 public:
  UnOp(std::string n);  // TODO: add type of unary operation here

  void gen(std::vector<Operation>& res_code) override;
  std::string name() const override {
    return name_;
  }
  std::string op() const override { return "-"; }

  Node* child {nullptr};

 private:
  std::string name_;
};

class AssignOp: public Node {
 public:
  AssignOp();

  void gen(std::vector<Operation>& res_code) override;
  std::string op() const override { return "="; }

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
