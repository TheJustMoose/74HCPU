#pragma once

class Node;
class AssignOp;
class BinOp;
class RelationalOp;
class UnMinus;
class IncrementOp;
class DereferenceOp;
class Name;
class VarDecl;

class Visitor {
 public:
  virtual void Visit(Node* n) {}
  virtual void Visit(AssignOp* op) {}
  virtual void Visit(BinOp* op) {}
  virtual void Visit(RelationalOp* op) {}
  virtual void Visit(UnMinus* op) {}
  virtual void Visit(IncrementOp* op) {}
  virtual void Visit(DereferenceOp* op) {}
  virtual void Visit(Name* n) {}
  virtual void Visit(VarDecl* n) {}
};
