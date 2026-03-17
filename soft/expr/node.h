#pragma once

#include "node_type.h"
#include "operation.h"

#include <string>
#include <vector>

class Node {
 public:
  Node(NodeType nt): type_(nt) {}

  virtual void gen(std::vector<Operation>& res_code) = 0;

  virtual std::string op() const { return ""; }

  virtual std::string name() const { return "??"; }

  NodeType type() const { return type_; }

 protected:
  NodeType type_ {ntUnknown};
};
