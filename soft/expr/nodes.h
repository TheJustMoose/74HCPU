#pragma once

#include "data_type.h"
#include "operation.h"
#include "names.h"
#include "node.h"
#include "node_type.h"
#include "visitor.h"

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

  void accept(Visitor* v) override { v->Visit(this); }

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

  void accept(Visitor* v) override { v->Visit(this); }

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

  void accept(Visitor* v) override {
    left->accept(v);
    right->accept(v);
    v->Visit(this);
  }

 private:
  std::string name_ {"bo"};
};

class RelationalOp: public Node {
 public:
  RelationalOp(Token t, std::string tmp_name);

  std::string name() const override { return tmp_name_; }
  std::string op() const override;

  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;

  DataType data_type() override {
    return dtBool;
  }

  void accept(Visitor* v) override {
    left->accept(v);  // TODO: check it
    right->accept(v);
    v->Visit(this);
  }

 private:
  std::string tmp_name_ {"ro"};
};

class UnMinus: public Node {
 public:
  UnMinus(std::string n);

  std::string name() const override { return name_; }
  std::string op() const override { return "-"; }

  std::unique_ptr<Node> child;

  DataType data_type() override;

  void accept(Visitor* v) override {
    child->accept(v);
    v->Visit(this);
  }

 private:
  std::string name_{"um"};
};

class IncrementOp: public Node {
 public:
  IncrementOp(std::unique_ptr<Name> nm);

  std::unique_ptr<Name> child;

  DataType data_type() override;

  void accept(Visitor* v) override {
    //child->accept(v);
    //v->Visit(this);
  }
};

class DereferenceOp: public Node {
 public:
  DereferenceOp(std::unique_ptr<Node> n);

  std::unique_ptr<Node> child;

  DataType data_type() override;

  void accept(Visitor* v) override {}
};

class AssignOp: public Node {
 public:
  AssignOp();

  std::string op() const override { return "="; }
  std::string name() const override { return left ? left->name() : "eq"; }

  DataType data_type() override;

  void accept(Visitor* v) override {
    left->accept(v);
    right->accept(v);
    v->Visit(this);
  }

  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
};

class AddressOf: public Node {
 public:
  AddressOf(std::unique_ptr<Node> n);

  std::string name() const override { return "addrof"; }
  std::string op() const override { return "#"; }

  DataType data_type() override;
  bool is_pointer() { return true; }

  void accept(Visitor* v) override {
    v->Visit(this);
  }

  std::unique_ptr<Node> child;
};

class VarDecl: public Node {
 public:
  VarDecl(DataType dt, bool is_ptr);

  uint8_t var_size();
  std::string name() const override { return "de"; }

  std::string var_names() const;

  DataType data_type() override { return data_type_; }
  bool is_pointer() { return is_pointer_; }

  void accept(Visitor* v) override { v->Visit(this); }

  std::vector<std::string> names {};
  std::map<std::string, int> values {};

 private:
  DataType data_type_ {dtNotInitialize};
  bool is_pointer_ {false};
};
