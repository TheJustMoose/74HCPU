#include <cctype>
#include <iostream>
#include <string>

using namespace std;

string input_string;
size_t idx {0};
int value {0};

enum Token {
  tPlus, tMinus, tNum, tName, tEnd
};

enum NodeType {
  // var   num    binary operations
  ntName, ntNum, ntSum, ntSub, //ntMul, ntDiv,
  ntUMinus,  // unary minus
  ntUnknown  // we do not know now what is it
};

class Node {
 public:
  Node(NodeType nt): type_(nt) {}

  virtual int res() { return 0; }

  NodeType type() {
    return type_;
  };

  static NodeType Token2NodeType(Token t) {
    if (t == tPlus)
      return ntSum;
    else if (t == tMinus)
      return ntSub;
    else
      return ntUnknown;
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

 private:
  int value_ {0};
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
      else
        cout << "Only add and sub operations supported right now" << endl;
    }
    return 0;
  }

  Node* left {nullptr};
  Node* right {nullptr};
};

class UnOp: public Node {
 public:
  UnOp(): Node(ntUMinus) {}

  Node* child {nullptr};
};

// это прототип, чтобы поиграть в деревья
// поэтому пока считаем, что числа и цифры - это одно и то же,
// из операций есть только плюс, и в строке нет пробелов,
// только цифры и плюсы!
//
Token GetToken() {
  cout << "GetToken(): ";
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

  cout << "tEnd(2)" << endl;
  return tEnd;
}

Node* prim() {
  Token t = GetToken();
  if (t == tNum) {
    cout << "Find num: " << value << endl;
    return new Num(value);
  } else
    return nullptr;
}

Node* expr() {
  Node* left = prim();
  Token t = GetToken();
  while (t == tPlus || t == tMinus) {
    Node* right = prim();
    BinOp* op = new BinOp(t);
    op->left = left;
    op->right = right;
    left = op;
    t = GetToken();
  }

  if (left)
    return left;

  cout << "Got Token == " << static_cast<int>(t) << endl;
  return nullptr;
}

int main(int argc, char* argv[]) {
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

  cout << "expr res: " << n->res() << endl;
  return 0;
}
