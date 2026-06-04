#pragma once

#include "data_type.h"
#include "node_type.h"
#include "operation.h"

#include <string>
#include <vector>

class Node {
 public:
  Node(NodeType nt): type_(nt) {}

  virtual ~Node() = default;

  virtual std::string op() const { return ""; }

  virtual std::string name() const { return "??"; }

  NodeType type() const { return type_; }

  virtual DataType data_type() = 0;

 protected:
  NodeType type_ {ntUnknown};
};
