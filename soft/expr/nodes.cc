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

Name::Name(string value)
  : Node(ntName), value_(value) {
}

BinOp::BinOp(Token t, string n)
  : Node(Token2NodeType(t)), name_(n) {
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

DataType BinOp::data_type() {
  if (!left || !right)
    return dtNotInitialize;

  DataType ldt = left->data_type();
  DataType rdt = right->data_type();
  return max(ldt, rdt);
}

UnMinus::UnMinus(string n)
  : Node(ntUMinus), name_(n) {
}

DataType UnMinus::data_type() {
  if (child)
    return child->data_type();
  else
    return dtNotInitialize;
}

AssignOp::AssignOp()
  : Node(ntAssign) {
}

DataType AssignOp::data_type() {
  if (right)
    return right->data_type();
  else
    return dtNotInitialize;
}

AddressOf::AddressOf(std::unique_ptr<Node> n)
  : Node(ntAddressOf), child(std::move(n)) {
}

DataType AddressOf::data_type() {
  if (child)
    return child->data_type();
  else
    return dtNotInitialize;
}

VarDecl::VarDecl(DataType dt, bool is_ptr)
  : Node(ntVarDecl), data_type_(dt), is_pointer_(is_ptr) {}

// This is not the C++!
// "char@ a, b; " means, that we have two pointers.
// ***** b has type char@ too. *****
// Yes, I use @ instead of *.
//
// In C++ "char* a, b;" means
// that you have pointer "a" and char "b".
uint8_t VarDecl::var_size() {
  return ::var_size(data_type(), is_pointer());
}

string VarDecl::var_names() const {
  string res;
  for (string s : names) {
    res += s;
    res += ",";
  }

  if (res.size() && *res.rbegin() == ',')
    res.resize(res.size() - 1);

  return res;
}
