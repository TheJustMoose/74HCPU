#include "print_tree.h"
#include "node.h"
#include "nodes.h"

#include <iostream>
#include <string>

using namespace std;

const int width = 80;

string dup(char c, int w) {
  string res(w, c);
  return res;
}

void Tree2List(Node* n) {
}

void PrintTree(Node* n) {
  if (!n)
    return;

  cout << dup(' ', width / 2) << n->op() << endl;

  if (BinOp* bop = dynamic_cast<BinOp*>(n)) {
  }

  if (AssignOp* aop = dynamic_cast<AssignOp*>(n)) {
  }
}
