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
/*
  tmp_name_ = new_tmp();
  cout << FuncGuard::stack_str() << tmp_name_ << " = " << value_ << endl;
  res_code.emplace_back(tmp_name_, "", to_string(value_), "", true);
*/
}

Name::Name(string value)
  : Node(ntName), value_(value) {
}

void Name::init_size(uint8_t size) {
  cached_size_ = size;
}

void Name::gen(vector<Operation>& res_code) {
  cout << FuncGuard::stack_str() << "Name object: " << value_ << endl;
}

string Name::name() {
  return value_;
}

uint8_t Name::cached_size() {
  return cached_size_;
}

BinOp::BinOp(Token t)
  : Node(Token2NodeType(t)) {
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
/*
    tmp_name_ = new_tmp();
    cout << FuncGuard::stack_str() << tmp_name_ << " = "
         << left->name() << " " << op() << " "
         << right->name() << endl;
*/
  }
  //res_code.emplace_back(tmp_name_, op(), left->name(), right->name(), true);
}

string BinOp::op() {
  switch (type()) {
    case ntSum: return "+";
    case ntSub: return "-";
    case ntMul: return "*";
    case ntDiv: return "/";
    default: return "";
  }
}

UnOp::UnOp()
  : Node(ntUMinus) {
}

void UnOp::gen(vector<Operation>& res_code) {
  child->gen(res_code);
/*
  tmp_name_ = new_tmp();
  cout << FuncGuard::stack_str() << tmp_name_ << " = "
       << op() << child->name() << " " << endl;
  res_code.emplace_back(tmp_name_, op(), "", child->name(), true);
*/
}

string UnOp::op() {
  return "-";
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
    cout << FuncGuard::stack_str() << "Error. left node is not Name class" << endl;
    return;
  }
/*
  cout << FuncGuard::stack_str()
       << name_node->name() << " = "
       << right->name() << endl;
  res_code.emplace_back(name_node->name(), "", right->name(), "");
*/
}

string AssignOp::op() {
  return "=";
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
