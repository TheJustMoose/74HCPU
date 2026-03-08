#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "func_guard.h"
#include "lexer.h"
#include "node.h"
#include "node_type.h"
#include "token.h"
#include "names.h"

using namespace std;

// Prototype to play with tree
//

/*
name -> id (a few letters)
prim -> num | name | (expr)
term -> term * prim
expr -> expr + term
assi -> assi = expr | name


term_rest -> * prim | E
term -> prim | term_rest

*/

Lexer lex;

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

Node* prim();
Node* term();
Node* expr();
Node* assign();
bool stmt();

class Num: public Node {
 public:
  Num(int value)
    : Node(ntNum), value_(value) {}

  int res() override {
    return value_;
  }

  void gen() override {
    tmp_name_ = new_tmp();
    cout << stack_str() << tmp_name_ << " = " << value_ << endl;
  }

 private:
  int value_ {0};
};

class Name: public Node {
 public:
  Name(string value)
    : Node(ntName), value_(value) {}

  int res() override {
    return 0;
  }

  void gen() override {
    cout << stack_str() << "Name object: " << value_ << endl;
  }

  string tmp_name() override {
    return value_;
  }

  string name() {
    return value_;
  }

 private:
  string value_ {0};
};

class BinOp: public Node {
 public:
  BinOp(Token t): Node(Token2NodeType(t)) {}

  int res() override {
    if (!left)
      cout << "left node is nullptr" << endl;
    else if (!right)
      cout << "right node is nullptr" << endl;
    else {
      switch (type()) {
        case ntSum: return left->res() + right->res();
        case ntSub: return left->res() - right->res();
        case ntMul: return left->res() * right->res();
        case ntDiv: return left->res() * right->res();
        default:
          cout << "Only add, sub, mul, div operations supported right now" << endl;
      }
    }

    return 0;
  }

  void gen() override {
    if (!left)
      cout << "left node is nullptr" << endl;
    else if (!right)
      cout << "right node is nullptr" << endl;
    else {
      left->gen();
      right->gen();
      tmp_name_ = new_tmp();
      cout << stack_str() << tmp_name_ << " = "
           << left->tmp_name() << " " << op() << " "
           << right->tmp_name() << endl;
    }
  }

  string op() override {
    switch (type()) {
      case ntSum: return "+";
      case ntSub: return "-";
      case ntMul: return "*";
      case ntDiv: return "/";
      default: return "";
    }
  }

  Node* left {nullptr};
  Node* right {nullptr};
};

class UnOp: public Node {
 public:
  UnOp(): Node(ntUMinus) {}

  int res() override {
    if (!child)
      cout << "child node is nullptr" << endl;
    return - child->res();
  }

  void gen() override {
    child->gen();
    tmp_name_ = new_tmp();
    cout << stack_str() << tmp_name_ << " = " << op()
         << child->tmp_name() << " " << endl;
  }

  string op() override {
    return "-";
  }

  Node* child {nullptr};
};

class AssignOp: public Node {
 public:
  AssignOp(): Node(ntAssign) {}

  int res() override {
    return right ? right->res() : 0;
  }

  void gen() override {
    right->gen();
    if (!left) {
      cout << stack_str() << "left == nullptr" << endl;
      return;
    }
    Name* name_node = dynamic_cast<Name*>(left);
    if (!name_node) {
      cout << stack_str() << "Error. left node is not Name class" << endl;
      return;
    }

    cout << stack_str() << name_node->name() << " = " << right->tmp_name() << endl;
  }

  string op() override {
    return "=";
  }

  Node* left {nullptr};
  Node* right {nullptr};
};

Node* prim() {
  FuncGuard fg("prim");
  Token t = lex.currentToken();
  if (t == tNum) {
    int val = lex.getIntValue();
    cout << stack_str() << "Find num: " << val << endl;
    lex.consume();
    return new Num(val);
  } else if (t == tName) {
    string str = lex.getStrValue();
    cout << stack_str() << "Find name: " << str << endl;
    lex.consume();
    return new Name(str);
  } else if (t == tMinus) {
    lex.consume();
    UnOp* n = new UnOp();
    n->child = prim();
    return n;
  } else if (t == tLBracket) {
    lex.consume();
    Node* n = expr();
    t = lex.currentToken();
    if (t != tRBracket)
      cout << stack_str() << "Error. Waiting for right bracket" << endl;
    else
      lex.consume();
    return n;
  } else {
    return nullptr;
  }
}

Node* term() {
  FuncGuard fg("term");
  Node* left = prim();
  if (!left)
    return nullptr;

  Token t = lex.currentToken();
  while (t == tMul || t == tDiv) {
    cout << stack_str() << "Okay. Found * or /" << endl;
    Token t_op = t;
    lex.consume();

    Node* right = prim();
    // prim have already called lex.consume();

    BinOp* op = new BinOp(t_op);
    op->left = left;
    op->right = right;
    left = op;
    t = lex.currentToken();
  }

  return left;
}

Node* expr() {
  FuncGuard fg("expr");
  Node* left = term();
  if (!left) {
    return nullptr;
  }

  Token t = lex.currentToken();
  while (t == tPlus || t == tMinus) {
    cout << stack_str() << "Okay. Found + or -" << endl;
    Token t_op = t;
    lex.consume();

    Node* right = term();
    // term have already called lex.consume(); in prim()

    BinOp* op = new BinOp(t_op);
    op->left = left;
    op->right = right;
    left = op;
    t = lex.currentToken();
  }

  return left;
}

Node* assign() {
  FuncGuard fg("assi");
  Node* left = expr();  // a
  if (!left)
    return nullptr;

  Token t = lex.currentToken();
  if (t == tEnd)
    return left;

  Node* res {nullptr};
  if (t == tEqual) {
    lex.consume();
    AssignOp* op = new AssignOp();  // =
    op->left = left;                // a =
    op->right = assign();           // a = assign(); // recursion
    res = op;
  } else {
    res = left;
  }

  return res;
}

vector<Node*> statements;

bool stmt() {
  FuncGuard fg("stmt");
  Node* n = assign();
  if (!n) {
    cout << stack_str() << "**** Error. assign() return nullptr ****" << endl;
    return false;
  }

  statements.push_back(n);

  Token t = lex.currentToken();
  while (true) {
    if (t == tSemicolon) {
      lex.consume();
      n = assign();
      if (!n)
        break;
      statements.push_back(n);
      t = lex.currentToken();
    } else {
      cout << stack_str() << "**** Error. Please add ';' to the end of statement ****" << endl;
      return false;
    }
  }

  return true;
}
