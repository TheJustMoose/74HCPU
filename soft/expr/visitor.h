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
  virtual void Visit(Node*) {}
  virtual void Visit(AssignOp*) {}
  virtual void Visit(BinOp*) {}
  virtual void Visit(RelationalOp*) {}
  virtual void Visit(UnMinus*) {}
  virtual void Visit(IncrementOp*) {}
  virtual void Visit(DereferenceOp*) {}
  virtual void Visit(Name*) {}
  virtual void Visit(AddressOf*) {}
  virtual void Visit(VarDecl*) {}
};
