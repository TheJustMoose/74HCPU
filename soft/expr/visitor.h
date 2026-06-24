#pragma once

class Node;
class AssignOp;
class BinOp;
class UnMinus;
class Name;
class VarDecl;

class Visitor {
 public:
  virtual void Visit(Node* n) {}
  virtual void Visit(AssignOp* op) {}
  virtual void Visit(BinOp* op) {}
  virtual void Visit(UnMinus* op) {}
  virtual void Visit(Name* n) {}
  virtual void Visit(VarDecl* n) {}
};
