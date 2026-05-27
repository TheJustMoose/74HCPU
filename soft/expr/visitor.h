#pragma once

class Node;
class AssignOp;
class BinOp;
class UnOp;
class Name;

class Visitor {
 public:
  virtual void Visit(Node* n) {}
  virtual void Visit(AssignOp* op) {}
  virtual void Visit(BinOp* op) {}
  virtual void Visit(UnOp* op) {}
  virtual void Visit(Name* n) {}
};
