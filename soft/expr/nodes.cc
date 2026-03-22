#include "nodes.h"

#include "func_guard.h"
#include "token2node_type.h"
#include "var_size.h"

#include <cctype>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

Num::Num(int value)
 : Node(ntNum), value_(value) {
}

void Num::gen(vector<Operation>& res_code) {
  // number value will be used by other nodes
}

Name::Name(string value)
  : Node(ntName), value_(value) {
}

void Name::init_size(uint8_t size) {
  cached_size_ = size;
}

void Name::gen(vector<Operation>& res_code) {
  // name value will be used by other nodes
}

uint8_t Name::cached_size() {
  return cached_size_;
}

BinOp::BinOp(Token t, string n)
  : Node(Token2NodeType(t)), name_(n) {
}

void BinOp::gen(vector<Operation>& res_code) {
  if (!left)
    cout << "left node is nullptr" << endl;
  else if (!right)
    cout << "right node is nullptr" << endl;
  else {
    left->gen(res_code);
    right->gen(res_code);
    // Okay, let's put operation result into temp variable
    cout << FuncGuard::stack_str() << name() << " = "
         << left->name() << " " << op() << " "
         << right->name() << endl;
  }
  bool r_is_n = right && (right->type() == ntNum);
  res_code.emplace_back(name(), op(), left->name(), r_is_n, right->name(), true);
}

string BinOp::op() const {
  switch (type()) {
    case ntSum: return "+";
    case ntSub: return "-";
    case ntMul: return "*";
    case ntDiv: return "/";
    default: return "";
  }
}

UnOp::UnOp(string n)
  : Node(ntUMinus), name_(n) {
}

void UnOp::gen(vector<Operation>& res_code) {
  child->gen(res_code);

  cout << FuncGuard::stack_str() << name_ << " = "
       << op() << child->name() << " " << endl;
  res_code.emplace_back(name_, op(), "", false, child->name(), true);
}

AssignOp::AssignOp()
  : Node(ntAssign) {
}

void AssignOp::gen(vector<Operation>& res_code) {
  right->gen(res_code);
  if (!left) {
    cout << FuncGuard::stack_str() << "left == nullptr" << endl;
    return;
  }

  Name* name_node = dynamic_cast<Name*>(left);
  if (!name_node) {
    cout << FuncGuard::stack_str() << "Error. left node is not variable (Name class)" << endl;
    return;
  }

  //left->gen(res_code);
  //right->gen(res_code);

  cout << FuncGuard::stack_str()
       << name_node->name() << " = "
       << right->name() << endl;

  bool r_is_n = right && (right->type() == ntNum);
  res_code.emplace_back(name_node->name(), "", right->name(), r_is_n, "");
}

VarDecl::VarDecl(DataType dt, bool is_ptr)
  : Node(ntVarDecl), data_type(dt), is_pointer(is_ptr) {}

void VarDecl::gen(vector<Operation>& res_code) {
  // This node is just variable declaration.
  // No machine code to implement it.
}

// This is not the C++!
// "char@ a, b; " means, that we have two pointers.
// ***** b has type char@ too. *****
// Yes, I use @ instead of *.
//
// In C++ "char* a, b;" means
// that you have pointer "a" and char "b".
uint8_t VarDecl::var_size() {
  return ::var_size(data_type, is_pointer);
}
