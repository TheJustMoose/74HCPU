#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "parser.h"

#include "data_type.h"
#include "func_guard.h"
#include "lexer.h"
#include "node.h"
#include "node_type.h"
#include "operation.h"
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

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

class Num: public Node {
 public:
  Num(int value)
    : Node(ntNum), value_(value) {}

  int res() override {
    return value_;
  }

  void gen(vector<Operation>& res_code) override {
    tmp_name_ = new_tmp();
    cout << FuncGuard::stack_str() << tmp_name_ << " = " << value_ << endl;
    res_code.emplace_back(tmp_name_, "", to_string(value_), "", true);
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

  void gen(vector<Operation>& res_code) override {
    cout << FuncGuard::stack_str() << "Name object: " << value_ << endl;
  }

  string tmp_name() override {
    return value_;
  }

  string name() {
    return value_;
  }

 private:
  string value_ {""};
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

  void gen(vector<Operation>& res_code) override {
    if (!left)
      cout << "left node is nullptr" << endl;
    else if (!right)
      cout << "right node is nullptr" << endl;
    else {
      left->gen(res_code);
      right->gen(res_code);
      tmp_name_ = new_tmp();
      cout << FuncGuard::stack_str() << tmp_name_ << " = "
           << left->tmp_name() << " " << op() << " "
           << right->tmp_name() << endl;
    }
    res_code.emplace_back(tmp_name_, op(), left->tmp_name(), right->tmp_name(), true);
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

  void gen(vector<Operation>& res_code) override {
    child->gen(res_code);
    tmp_name_ = new_tmp();
    cout << FuncGuard::stack_str() << tmp_name_ << " = " << op()
         << child->tmp_name() << " " << endl;
    res_code.emplace_back(tmp_name_, op(), "", child->tmp_name(), true);
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

  void gen(vector<Operation>& res_code) override {
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

    cout << FuncGuard::stack_str()
         << name_node->name() << " = "
         << right->tmp_name() << endl;
    res_code.emplace_back(name_node->name(), "", right->tmp_name(), "");
  }

  string op() override {
    return "=";
  }

  Node* left {nullptr};
  Node* right {nullptr};
};

class VarDecl: public Node {
 public:
  VarDecl(DataType dt, bool is_ptr)
    : Node(ntVarDecl), data_type(dt), is_pointer(is_ptr) {}

  int res() override { return 0; }

  void gen(vector<Operation>& res_code) override {
    // This node is just variable declaration.
    // No machine code to implement it.
  }

  // This is not C++!
  // "char@ a, b; " means, that we have two pointers.
  // ***** b has type char@ too. *****
  // Yes, I use @ instead of *.
  //
  // In C++ "char* a, b; means
  // that you have pointer "a" and char "b".

  DataType data_type {dtNotInitialize};
  bool is_pointer {false};
  vector<string> names {};
};

Node* prim() {
  FuncGuard fg("prim");
  Token t = Lexer::instance().currentToken();
  if (t == tNum) {
    int val = Lexer::instance().getIntValue();
    cout << FuncGuard::stack_str() << "Find num: " << val << endl;
    Lexer::instance().consume();
    return new Num(val);
  } else if (t == tName) {
    string str = Lexer::instance().getStrValue();
    cout << FuncGuard::stack_str() << "Find name: " << str << endl;
    Lexer::instance().consume();
    return new Name(str);
  } else if (t == tMinus) {
    Lexer::instance().consume();
    UnOp* n = new UnOp();
    n->child = prim();
    return n;
  } else if (t == tLBracket) {
    Lexer::instance().consume();
    Node* n = expr();
    t = Lexer::instance().currentToken();
    if (t != tRBracket)
      cout << FuncGuard::stack_str()
           << "Error. Waiting for right bracket" << endl;
    else
      Lexer::instance().consume();
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

  Token t = Lexer::instance().currentToken();
  while (t == tMul || t == tDiv) {
    cout << FuncGuard::stack_str() << "Okay. Found * or /" << endl;
    Token t_op = t;
    Lexer::instance().consume();

    Node* right = prim();
    // prim have already called Lexer::instance().consume();

    BinOp* op = new BinOp(t_op);
    op->left = left;
    op->right = right;
    left = op;
    t = Lexer::instance().currentToken();
  }

  return left;
}

Node* expr() {
  FuncGuard fg("expr");
  Node* left = term();
  if (!left) {
    return nullptr;
  }

  Token t = Lexer::instance().currentToken();
  while (t == tPlus || t == tMinus) {
    cout << FuncGuard::stack_str() << "Okay. Found + or -" << endl;
    Token t_op = t;
    Lexer::instance().consume();

    Node* right = term();
    // term have already called Lexer::instance().consume(); in prim()

    BinOp* op = new BinOp(t_op);
    op->left = left;
    op->right = right;
    left = op;
    t = Lexer::instance().currentToken();
  }

  return left;
}

Node* assign() {
  FuncGuard fg("assi");
  Node* left = expr();  // a
  if (!left)
    return nullptr;

  Token t = Lexer::instance().currentToken();
  if (t == tEnd)
    return left;

  Node* res {nullptr};
  if (t == tEqual) {
    Lexer::instance().consume();
    AssignOp* op = new AssignOp();  // =
    op->left = left;                // a =
    op->right = assign();           // a = assign(); // recursion
    res = op;
  } else {
    res = left;
  }

  return res;
}

Node* declare() {
  FuncGuard fg("decl");

  Token t = Lexer::instance().currentToken();
  if (t == tEnd)
    return nullptr;

  if (t != tInt) {
    cout << "This token is not tInt" << endl;
    return nullptr;
  }

  Lexer::instance().consume();  // skip int

  bool is_ptr = false;
  t = Lexer::instance().currentToken();
  if (t == tAtSign) {  // pointer sign (not asterisk)
    is_ptr = true;
    Lexer::instance().consume();
  }

  VarDecl* n = new VarDecl(dtInt, is_ptr);  // Okay, try to find variable name[s]

  t = Lexer::instance().currentToken();
  while (t == tName) {
    cout << "Variable name was found!" << endl;
    string var_name = Lexer::instance().getStrValue();
    n->names.push_back(var_name);
    cout << "Int variable \"" << var_name << "\" was declared" << endl;

    // What about variable initialization?

    Lexer::instance().consume();  // skip var name
    t = Lexer::instance().currentToken();
    if (t == tComma) {
      Lexer::instance().consume();  // skip comma
      t = Lexer::instance().currentToken();
    } else if (t == tSemicolon) {  // replace it to comma later
      cout << "Semicolon found. Leave it for stmts() func" << endl;
      return n;
    } else {
      cout << "Error. Please add ';' to the end of declaration" << endl;
      cout << "Got token: " << (int)t << endl;
      return nullptr;
    }
  }

  return nullptr;
}

Node* stmt() {
  // stmt -> assigh | declare
  Node* n = declare();
  return n ? n : assign();
}

bool stmts(vector<Node*>& statements) {
  FuncGuard fg("stmt");

  Node* n = stmt();
  Token t = Lexer::instance().currentToken();
  while (n) {
    if (t == tSemicolon) {
      if (!n) {
        cout << "Node == null was returned from stmt()" << endl;
        break;
      }
      statements.push_back(n);

      Lexer::instance().consume();
      n = stmt();
      t = Lexer::instance().currentToken();
    } else {
      cout << FuncGuard::stack_str()
           << "**** Error. Please add ';' to the end of statement ****" << endl;
      return false;
    }
  }

  return true;
}
