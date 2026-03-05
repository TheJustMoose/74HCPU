#include <cctype>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

// Prototype to play with tree
//

using namespace std;

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


class Lexer {
 public:
  Lexer(string s): input_string_(s) {}

  char currentToken() {
    return (idx_ < input_string_.size()) ?
        input_string_[idx_] : 0;
  }

  void consume() {
    if (idx_ < input_string_.size())
      idx_++;
  }

 private:
  string input_string_ {};
  size_t idx_ {0};
};

int value {0};
string name {""};

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

enum Token {
  tPlus, tMinus, tMul, tDiv,
  tNum, tName,
  tLBracket, tRBracket, tEqual, tSemicolon,
  tEnd
};

map<Token, string> TokenName {
  {tPlus, "tPlus"},
  {tMinus, "tMinus"},
  {tMul, "tMul"},
  {tDiv, "tDiv"},
  {tNum, "tNum"},
  {tName, "tName"},
  {tLBracket, "tLBracket"},
  {tRBracket, "tRBracket"},
  {tEqual, "tEqual"},
  {tSemicolon, "tSemicolon"},
  {tEnd, "tEnd"},
};

enum NodeType {
  // var   num    binary operations
  ntName, ntNum, ntSum, ntSub, ntMul, ntDiv,
  ntUMinus,   // unary minus
  ntAssign,   // a = b;
  ntUnknown,  // we do not know now what is it
  ntNull
};

map<NodeType, string> NodeName {
  {ntName, "ntName"},
  {ntNum, "ntNum"},
  {ntSum, "ntSum"},
  {ntSub, "ntSub"},
  {ntMul, "ntMul"},
  {ntDiv, "ntDiv"},
  {ntUMinus, "ntUMinus"},
  {ntAssign, "ntAssign"},
  {ntUnknown, "ntUnknown"},
  {ntNull, "ntNull"},
};

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

Token GetToken() {
  FuncGuard fg("getT");
  cout << stack_str() << "GetToken(): ";
  if (idx >= input_string.size()) {
    cout << "tEnd" << endl;
    return tEnd;
  }

  char c = input_string[idx++];
  if (isdigit(c)) {
    value = 0;
    while (isdigit(c)) {
      int dig = c - '0';
      value = value*10 + dig;
      c = input_string[idx++];
    }
    idx--;
    cout << "tNum" << endl;
    return tNum;
  }

  if (isalpha(c)) {
    name = c;
    cout << "tName: " << name << endl;
    return tName;
  }

  switch (c) {
    case '+': cout << TokenName[tPlus] << endl; return tPlus;
    case '-': cout << TokenName[tMinus] << endl; return tMinus;
    case '*': cout << TokenName[tMul] << endl; return tMul;
    case '/': cout << TokenName[tDiv] << endl; return tDiv;
    case '(': cout << TokenName[tLBracket] << endl; return tLBracket;
    case ')': cout << TokenName[tRBracket] << endl; return tRBracket;
    case '=': cout << TokenName[tEqual] << endl; return tEqual;
    case ';': cout << TokenName[tSemicolon] << endl; return tSemicolon;
  }

  cout << stack_str() << "Unknown token: " << c << endl;
  cout << "tEnd(2)" << endl;
  return tEnd;
}

void ReturnToken() {
  FuncGuard fg("retT");
  cout << stack_str() << "ReturnToken()" << endl;
  if (idx > 0) {  //  && idx < input_string.size()
    idx--;
    //cout << "idx: " << (idx + 1) << " --> " << idx << endl;
    //cout << "tok: " << input_string[idx + 1] << " --> " << input_string[idx] << endl;
  } else
    cout << "...Nothing to return. idx == 0" << endl;
}

Node* prim() {
  FuncGuard fg("prim");
  Token t = GetToken();
  if (t == tNum) {
    cout << stack_str() << "Find num: " << value << endl;
    return new Num(value);
  } else if (t == tName) {
    cout << stack_str() << "Find name: " << name << endl;
    return new Name(name);
  } else if (t == tMinus) {
    UnOp* n = new UnOp();
    n->child = prim();
    return n;
  } else if (t == tLBracket) {
    Node* n = expr();
    t = GetToken();
    if (t != tRBracket) {
      ReturnToken();
      cout << "Error. Waiting for right bracket" << endl;
    }
    return n;
  } else {
    cout << "Before return " << TokenName[t] << " token" << endl;
    //ReturnToken();
    return nullptr;
  }
}

Node* term() {
  FuncGuard fg("term");
  Node* left = prim();
  if (!left)
    return nullptr;

  Token t = GetToken();
  while (t == tMul || t == tDiv) {
    Node* right = prim();
    BinOp* op = new BinOp(t);
    op->left = left;
    op->right = right;
    left = op;
    t = GetToken();
  }

  ReturnToken();
  return left;
}

Node* expr() {
  FuncGuard fg("expr");
  Node* left = term();
  if (!left)
    return nullptr;

  Token t = GetToken();
  while (t == tPlus || t == tMinus) {
    Node* right = term();
    BinOp* op = new BinOp(t);
    op->left = left;
    op->right = right;
    left = op;
    t = GetToken();
  }

  ReturnToken();  // check it!
  if (!left)
    cout << "Got Token == " << static_cast<int>(t) << endl;
  return left;
}

Node* assign() {
  FuncGuard fg("assi");
  Node* left = expr();  // a
  if (!left)
    return nullptr;

  cout << "  left/expr()->type(): " << NodeName[left->type()] << endl;

  Token t = GetToken();
  if (t == tEnd)
    return left;

  cout << "  right token -> (" << static_cast<int>(t)
       << ", " << TokenName[t] << ")" << endl;

  Node* res {nullptr};
  if (t == tEqual) {
    AssignOp* op = new AssignOp();  // =
    cout << "  AssignOp was created..." << endl;
    op->left = left;                // a =
    op->right = assign();           // a = assign(); // recursion
    res = op;
  } else {
    res = left;
    ReturnToken();
  }

  return res;
}

vector<Node*> statements;

bool stmt() {
  FuncGuard fg("stmt");
  Node* n = assign();
  if (!n) {
    cout << "assign return nullptr, I don't know why" << endl;
    return false;
  }

  Token t = GetToken();
  cout << "Token after \"n = assign()\" -> (" << static_cast<int>(t)
       << ", " << TokenName[t] << ")" << endl;

  statements.push_back(n);

  while (t == tSemicolon) {
    n = assign();
    if (!n)
      break;
    statements.push_back(n);
    t = GetToken();
  }

/*
    cout << "**** HORAY! ****" << endl;
  else
    cout << "**** Error. Please add ';' to the end of statement ****" << endl;
*/
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
    cout << "Try to process: " << argv[1] << "..." << endl;
    input_string = argv[1];
    cout << "input_string.size(): " << input_string.size() << endl;
  } else {
    cout << "Using: plus.exe \"1+2+3\"" << endl;
    return 1;
  }

  stmt();  // have to check result
  for (Node* n : statements) {
    n->gen();
  }

  /*if (n->type() == ntAssign) {
    cout << "Root node is ntAssign" << endl;
    AssignOp* assign = dynamic_cast<AssignOp*>(n);
    if (assign) {
      if (AssignOp* t1 = assign) {
        NodeType ntLeft = t1->left ? t1->left->type() : ntNull;
        cout << "  t1->left->type(): " << NodeName[ntLeft] << endl;
        NodeType ntRight = t1->right ? t1->right->type() : ntNull;
        cout << "  t1->right->type(): " << NodeName[ntRight] << endl;
        Name* n {nullptr};
        if (t1->left && (n = dynamic_cast<Name*>(t1->left)))
          cout << "  left->name(): " << (n ? n->name() : string("left is nullptr")) << endl;
      }

      if (AssignOp* t1 = dynamic_cast<AssignOp*>(assign->right)) {
        NodeType ntLeft = t1->left ? t1->left->type() : ntNull;
        cout << "  t1->left->type(): " << NodeName[ntLeft] << endl;
        NodeType ntRight = t1->right ? t1->right->type() : ntNull;
        cout << "  t1->right->type(): " << NodeName[ntRight] << endl;
        Name* n {nullptr};
        if (t1->left && (n = dynamic_cast<Name*>(t1->left)))
          cout << "  left->name(): " << (n ? n->name() : string("left is nullptr")) << endl;
      }
    } else {
      cout << "dynamic_cast<AssignOp*>(n); return nullptr Oo" << endl;
    }
  } else {
    cout << "Root node is " << NodeName[n->type()] << endl;
  }*/

  //n->gen();  // enum tree items, generate three-address code 

  //cout << "expr res: " << n->res() << endl;
  cout << "main finished" << endl;
  return 0;
}
