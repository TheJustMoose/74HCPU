#pragma once

#include "node_type.h"
#include "operation.h"

#include <string>

class Node {
 public:
  Node(NodeType nt): type_(nt) {}

  virtual int res() = 0;
  virtual Operation gen() = 0;

  virtual std::string op() { return ""; }
  virtual std::string tmp_name() {
    return tmp_name_;
  }

  NodeType type() {
    return type_;
  };

 protected:
  NodeType type_ {ntUnknown};
  std::string tmp_name_ {};
};
