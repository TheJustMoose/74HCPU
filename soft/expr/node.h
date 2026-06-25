#pragma once

#include "data_type.h"
#include "node_type.h"

#include <string>

class Visitor;

class Node {
 public:
  Node(NodeType nt): node_type_(nt) {}

  virtual ~Node() = default;

  virtual std::string op() const { return ""; }

  virtual std::string name() const { return "??"; }

  NodeType node_type() const { return node_type_; }

  virtual DataType data_type() = 0;

  virtual void accept(Visitor* v) = 0;

 protected:
  NodeType node_type_ {ntUnknown};
};
