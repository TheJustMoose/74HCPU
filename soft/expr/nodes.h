#pragma once

#include "data_type.h"
#include "operation.h"
#include "names.h"
#include "node.h"
#include "node_type.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Num: public Node {
 public:
  Num(int value);

  std::string name() const override {
    return std::to_string(value_);
  }
  int value() const { return value_; }

  DataType data_type() override { return dtByte; }

 private:
  int value_ {0};
};

class Name: public Node {
 public:
  Name(std::string value);
  void init_data_type(DataType dt) {
    data_type_ = dt;
  }

  std::string name() const override { return value_; }

  DataType data_type() override { return data_type_; }

 private:
  std::string value_ {""};
  DataType data_type_ {dtNotInitialize};
};

class BinOp: public Node {
 public:
  BinOp(Token t, std::string n);

  std::string name() const override { return name_; }
  std::string op() const override;

  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;

  DataType data_type() override;

 private:
  std::string name_ {"bo"};
};

class UnOp: public Node {
 public:
  UnOp(std::string n);  // TODO: add type of unary operation here

  std::string name() const override { return name_; }
  std::string op() const override { return "-"; }

  std::unique_ptr<Node> child;

  DataType data_type() override;

 private:
  std::string name_{"um"};
};

class AssignOp: public Node {
 public:
  AssignOp();

  std::string op() const override { return "="; }
  std::string name() const override { return left ? left->name() : "eq"; }

  DataType data_type() override;

  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
};

class VarDecl: public Node {
 public:
  VarDecl(DataType dt, bool is_ptr);

  uint8_t var_size();
  std::string name() const override { return "de"; }

  std::string var_names() const;

  DataType data_type() override { return data_type_; }
  bool is_pointer() { return is_pointer_; }

  std::vector<std::string> names {};
  std::map<std::string, int> values {};

 private:
  DataType data_type_ {dtNotInitialize};
  bool is_pointer_ {false};
};
