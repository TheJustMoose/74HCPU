#include "print_tree.h"
#include "names.h"
#include "node.h"
#include "node_type.h"
#include "nodes.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

const int width = 80;

string dup(char c, int w) {
  string res(w, c);
  return res;
}

vector<Node*> gLinearTree;

void Tree2List(Node* n, int lvl = 0) {
  NodeType nt = n->type();
  cout << "node type: " << GetNodeTypeName(nt) << ", lvl: " << lvl << endl;
  gLinearTree.push_back(n);

  if (BinOp* bop = dynamic_cast<BinOp*>(n)) {
    Tree2List(bop->left, lvl + 1);
    Tree2List(bop->right, lvl + 1);
  }

  if (AssignOp* aop = dynamic_cast<AssignOp*>(n)) {
    Tree2List(aop->left, lvl + 1);
    Tree2List(aop->right, lvl + 1);
  }
}

void PrintTree(Node* n) {
  if (!n)
    return;

  //cout << dup(' ', width / 2) << n->op() << endl;
  Tree2List(n);
  cout << endl;
}
