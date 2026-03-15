#include "print_tree.h"
#include "names.h"
#include "node.h"
#include "node_type.h"
#include "nodes.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

const int kWidth = 80;

string dup(char c, int w) {
  string res(w, c);
  return res;
}

vector<Node*> gLinearTree;
map<const Node*, int> gNodeLevel;

void Tree2List(Node* n, int lvl = 0) {
  gLinearTree.push_back(n);
  gNodeLevel[n] = lvl;

  if (BinOp* bop = dynamic_cast<BinOp*>(n)) {
    Tree2List(bop->left, lvl + 1);
    Tree2List(bop->right, lvl + 1);
  } else if (AssignOp* aop = dynamic_cast<AssignOp*>(n)) {
    Tree2List(aop->left, lvl + 1);
    Tree2List(aop->right, lvl + 1);
  } else {
    cout << "Node: " << GetNodeTypeName(n->type()) << endl;
  }
}

void PrintTree(Node* n) {
  if (!n)
    return;

  //cout << dup(' ', width / 2) << n->op() << endl;
  gLinearTree.clear();
  gNodeLevel.clear();

  Tree2List(n);
  cout << endl;

  int last_lvl {0};
  int nodes_on_lvl {0};
  int w = kWidth / 2;
  for (const Node* n: gLinearTree) {
    int lvl = gNodeLevel[n];
    if (last_lvl != lvl) {
      cout << "n on lvl: " << nodes_on_lvl << endl;
      last_lvl = lvl;
      nodes_on_lvl = 0;
      w /= 2;
    } else {
      nodes_on_lvl++;
    }

    cout << dup(' ', w) << GetNodeTypeName(n->type()) << " (" << lvl << ")" << dup(' ', 10);
  }

  cout << endl;
  cout << dup('-', kWidth) << endl;
}
