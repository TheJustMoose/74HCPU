#pragma once

class AddressOf;
class AssignOp;
class BinOp;
class DereferenceOp;
class IfStatement;
class IncrementOp;
class Name;
class Node;
class RelationalOp;
class UnMinus;
class VarDecl;

class Visitor {
 public:
  virtual void Visit(AddressOf*) {}
  virtual void Visit(AssignOp*) {}
  virtual void Visit(BinOp*) {}
  virtual void Visit(DereferenceOp*) {}
  virtual void Visit(IncrementOp*) {}
  virtual void Visit(IfStatement*) {}
  virtual void Visit(Name*) {}
  virtual void Visit(Node*) {}
  virtual void Visit(RelationalOp*) {}
  virtual void Visit(UnMinus*) {}
  virtual void Visit(VarDecl*) {}
};
