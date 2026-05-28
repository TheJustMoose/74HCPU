#include "enum_tree.h"
#include "node.h"
#include "nodes.h"
#include "visitor.h"

void EnumTree(Node* n, Visitor* v) {
  if (AssignOp* op = dynamic_cast<AssignOp*>(n)) {
    EnumTree(op->left.get(), v);
    EnumTree(op->right.get(), v);
    v->Visit(op);
  } else if (BinOp* op = dynamic_cast<BinOp*>(n)) {
    EnumTree(op->left.get(), v);
    EnumTree(op->right.get(), v);
    v->Visit(op);
  } else if (UnOp* op = dynamic_cast<UnOp*>(n)) {
    EnumTree(op->child.get(), v);
    v->Visit(op);
  } else if (Name* nm = dynamic_cast<Name*>(n)) {
    v->Visit(nm);
  } else {
    v->Visit(n);
  }
}
