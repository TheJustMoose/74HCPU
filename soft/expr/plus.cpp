#include <cctype>
#include <iostream>
#include <string>

using namespace std;

string input_string;
size_t idx {0};
int value {0};

enum Token {
  tPlus, tNum, tName, tEnd
};

enum NodeType {
  // var   num    binary operations
  ntName, ntNum, ntSum, //ntSub, ntMul, ntDiv,
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
  BinOp(): Node(ntSum) {}

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
  cout << "GetToken()" << endl;
  if (idx >= input_string.size())
    return tEnd;

  char c = input_string[idx++];
  if (isdigit(c)) {
    value = c - '0';
    return tNum;
  }

  if (isalpha(c))
    return tName;

  if (c == '+')
    return tPlus;

  return tEnd;
}

Node* expr() {
  Token t = GetToken();
  if (t == tNum) {
    cout << "Find num: " << value << endl;
    return new Num(value);
  } else
    cout << "Got Token == " << static_cast<int>(t) << endl;
  return 0;
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

  if (n->type() == ntNum)
    cout << "res: " << n->res() << endl;
  else
    cout << "type: " << n->type() << endl;

  return 0;
}
