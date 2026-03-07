#include <cctype>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

#include "lexer.h"
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

stack<string> call_stack {};

string stack_str() {
  stack<string> s = call_stack;
  string res;
  while (!s.empty()) {
    res = s.top() + string(">") + res;
    s.pop();
  }
  res += " ";
  return res;
}

class FuncGuard {
 public:
  FuncGuard(string n) {
    call_stack.push(n);
  }

  ~FuncGuard() {
    call_stack.pop();
  }
};

Lexer lex;

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

static NodeType Token2NodeType(Token t) {
  switch (t) {
    case tPlus: return ntSum;
    case tMinus: return ntSub;
    case tMul: return ntMul;
    case tDiv: return ntDiv;
    case tEqual: return ntAssign;
    default: return ntUnknown;
  }
}

class Node {
 public:
  Node(NodeType nt): type_(nt) {}

  virtual int res() = 0;
  virtual void gen() = 0;

  virtual string op() { return ""; }
  virtual string tmp_name() {
    return tmp_name_;
  }

  NodeType type() {
    return type_;
  };

 protected:
  NodeType type_ {ntUnknown};
  string tmp_name_ {};
};

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
    cout << stack_str() << "Okay, this is a Name object: " << value_ << endl;
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
  cout << stack_str() << "token: " << static_cast<int>(t) << endl;
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
    cout << stack_str() << "Before return " << GetTokenName(t) << " token" << endl;
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
    cout << stack_str() << "term() return nullptr" << endl;
    return nullptr;
  }

  Token t = lex.currentToken();
  while (t == tPlus || t == tMinus) {
    cout << stack_str() << "Okay. Found + or -" << endl;
    Token t_op = t;
    lex.consume();

    Node* right = term();
    // term have already called lex.consume(); in prim()
    if (right)
      cout << stack_str()
           << "right (part of sum) is not null, right->type() is "
           << GetNodeTypeName(right->type()) << endl;

    BinOp* op = new BinOp(t_op);
    op->left = left;
    op->right = right;
    left = op;
    t = lex.currentToken();
  }

  if (!left)
    cout << stack_str() << "Got Token == " << static_cast<int>(t) << endl;
  return left;
}

Node* assign() {
  FuncGuard fg("assi");
  Node* left = expr();  // a
  if (!left)
    return nullptr;

  cout << stack_str() << "  left/expr()->type(): "
                      << GetNodeTypeName(left->type()) << endl;

  Token t = lex.currentToken();
  if (t == tEnd)
    return left;

  cout << stack_str() << "  right token -> (" << static_cast<int>(t)
                      << ", " << GetTokenName(t) << ")" << endl;

  Node* res {nullptr};
  if (t == tEqual) {
    lex.consume();
    AssignOp* op = new AssignOp();  // =
    cout << stack_str() << "  AssignOp was created..." << endl;
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

int main(int argc, char* argv[]) {
  FuncGuard fg("main");
  cout << "stack: " << stack_str() << endl;
  cout << "argc: " << argc << endl;
  for (int i = 0; i < argc; i++) {
    cout << "i: " << i << ", arg[i]: " << argv[i] << endl;
  }
  cout << endl;

  if (argc == 2 && argv[1]) {
    cout << "Try to process: \"" << argv[1] << "\"" << endl;
    string s {argv[1]};
    lex.setInputString(s);
    cout << "input_string.size(): " << s.size() << endl;
  } else {
    cout << "Using: plus.exe \"1+2+3\"" << endl;
    return 1;
  }

  if (!stmt())
    return 1;

  for (Node* n : statements) {
    n->gen();  // enum tree items, generate three-address code 
    cout << "expr res: " << n->res() << endl;
  }

  cout << "main finished" << endl;
  return 0;
}
