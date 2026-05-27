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

void Name::gen(vector<Operation>& res_code) {
  // name value will be used by other nodes
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
  bool l_is_n = left && (left->type() == ntNum);

  NumPos np {npNone};
  if (r_is_n && l_is_n)
    np = npBoth;
  else if (r_is_n)
    np = npRight;
  else if (l_is_n)
    np = npLeft;

  res_code.emplace_back(name(), op(), left->name(), np, right->name(), true);
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

UnOp::UnOp(string n)
  : Node(ntUMinus), name_(n) {
}

void UnOp::gen(vector<Operation>& res_code) {
  child->gen(res_code);

  // the negative value is stored in the tree in two nodes: ntUMinus + ntNum
  bool is_num = child && (child->type() == ntNum);
  cout << FuncGuard::stack_str() << name_ << " = "
       << op() << child->name() << " " << endl;
  NumPos np = is_num ? npRight : npNone;
  res_code.emplace_back(name_, op(), "", np, child->name(), true);
}

DataType UnOp::data_type() {
  if (child)
    return child->data_type();
  else
    return dtNotInitialize;
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

  Name* name_node = dynamic_cast<Name*>(left.get());
  if (!name_node) {
    cout << FuncGuard::stack_str() << "Error. left node is not variable (Name class)" << endl;
    return;
  }

  //left->gen(res_code);
  //right->gen(res_code);

  cout << FuncGuard::stack_str()
       << name_node->name() << " = "
       << right->name() << endl;

  bool is_num = right && (right->type() == ntNum);
  NumPos np = is_num ? npLeft : npNone;
  res_code.emplace_back(name_node->name(), "", right->name(), np, "");
}

DataType AssignOp::data_type() {
  if (right)
    return right->data_type();
  else
    return dtNotInitialize;
}

VarDecl::VarDecl(DataType dt, bool is_ptr)
  : Node(ntVarDecl), data_type_(dt), is_pointer_(is_ptr) {}

void VarDecl::gen(vector<Operation>& res_code) {
  for (string n : names)
    if (values.find(n) != values.end())
      res_code.emplace_back(n, "", to_string(values[n]), npLeft, "");
}

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
