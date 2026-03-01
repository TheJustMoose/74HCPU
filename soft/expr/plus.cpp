#include <cctype>
#include <iostream>
#include <stack>
#include <string>

// это прототип, чтобы поиграть в деревья
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

string input_string;
size_t idx {0};
int value {0};

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

enum Token {
  tPlus, tMinus, tMul, tDiv, tNum, tName, tEnd
};

enum NodeType {
  // var   num    binary operations
  ntName, ntNum, ntSum, ntSub, ntMul, ntDiv,
  ntUMinus,  // unary minus
  ntUnknown  // we do not know now what is it
};

class Node {
 public:
  Node(NodeType nt): type_(nt) {}

  virtual int res() { return 0; }

  virtual void gen() {}

  virtual string op() { return ""; }
  virtual string tmp_name() { return ""; }

  NodeType type() {
    return type_;
  };

  static NodeType Token2NodeType(Token t) {
    switch (t) {
      case tPlus: return ntSum;
      case tMinus: return ntSub;
      case tMul: return ntMul;
      case tDiv: return ntDiv;
      default: return ntUnknown;
    }
  }

 private:
  NodeType type_ {ntUnknown};
};

class Num: public Node {
 public:
  Num(int value)
    : Node(ntNum), value_(value) {
  }

  int res() override {
    return value_;
  }

  void gen() override {
    tmp_name_ = new_tmp();
    cout << stack_str() << tmp_name_ << " = " << value_ << endl;
  }

  string tmp_name() override {
    return tmp_name_;
  }

 private:
  int value_ {0};
  string tmp_name_ {};
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
      if (type() == ntSum)
        return left->res() + right->res();
      else if (type() == ntSub)
        return left->res() - right->res();
      else if (type() == ntMul)
        return left->res() * right->res();
      else if (type() == ntDiv)
        return left->res() / right->res();
      else
        cout << "Only add, sub, mul, div operations supported right now" << endl;
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
    if (type() == ntSum)
      return "+";
    else if (type() == ntSub)
      return "-";
    else if (type() == ntMul)
      return "*";
    else if (type() == ntDiv)
      return "/";
    else
      return "";
  }

  string tmp_name() override {
    return tmp_name_;
  }

  Node* left {nullptr};
  Node* right {nullptr};

 private:
  string tmp_name_ {};
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

  string tmp_name() override {
    return tmp_name_;
  }

  Node* child {nullptr};

 private:
  string tmp_name_ {};
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
    cout << "tName" << endl;
    return tName;
  }

  if (c == '+') {
    cout << "tPlus" << endl;
    return tPlus;
  }

  if (c == '-') {
    cout << "tMinus" << endl;
    return tMinus;
  }

  if (c == '*') {
    cout << "tMul" << endl;
    return tMul;
  }

  if (c == '/') {
    cout << "tDiv" << endl;
    return tDiv;
  }

  cout << stack_str() << "Unknown token: " << c << endl;
  cout << "tEnd(2)" << endl;
  return tEnd;
}

void ReturnToken() {
  FuncGuard fg("retT");
  cout << stack_str() << "ReturnToken()" << endl;
  if (idx > 0 && idx < input_string.size())
    idx--;
  else
    cout << "...Nothing to return. We are at the npos of input string" << endl;
}

Node* prim() {
  FuncGuard fg("prim");
  Token t = GetToken();
  if (t == tNum) {
    cout << stack_str() << "Find num: " << value << endl;
    return new Num(value);
  } else if (t == tMinus) {
    UnOp* n = new UnOp();
    n->child = prim();
    return n;
  } else {
    ReturnToken();  // check it!
    return nullptr;
  }
}

Node* term() {
  FuncGuard fg("term");
  Node* left = prim();
  Token t = GetToken();
  while (t == tMul || t == tDiv) {
    Node* right = prim();
    BinOp* op = new BinOp(t);
    op->left = left;
    op->right = right;
    left = op;
    t = GetToken();
  }

  if (t != tMul && t != tDiv)  // token was not used!
    ReturnToken();

  return left;
}

Node* expr() {
  FuncGuard fg("expr");
  Node* left = term();
  Token t = GetToken();
  while (t == tPlus || t == tMinus) {
    Node* right = term();
    BinOp* op = new BinOp(t);
    op->left = left;
    op->right = right;
    left = op;
    t = GetToken();
  }

  if (t != tPlus && t != tMinus)  // token was not used!
    ReturnToken();  // check it!

  if (!left)
    cout << "Got Token == " << static_cast<int>(t) << endl;
  return left;
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

  Node* n = expr();
  if (!n) {
    cout << "Node* is nullptr" << endl;
    return 1;
  }

  n->gen();  // проходим по дереву, генерируем трёхадресный (?) код!
  cout << "expr res: " << n->res() << endl;

  cout << "main finished" << endl;
  return 0;
}
