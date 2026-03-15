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

const int width = 80;

string dup(char c, int w) {
  string res(w, c);
  return res;
}

vector<Node*> gLinearTree;
map<const Node*, int> gNodeLevel;
//map<int, int> gNodesOnLevel;

void Tree2List(Node* n, int lvl = 0) {
  gLinearTree.push_back(n);
  gNodeLevel[n] = lvl;

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

  int last_lvl {0};
  int nodes_on_lvl {0};
  for (const Node* n: gLinearTree) {
    int lvl = gNodeLevel[n];
    if (last_lvl != lvl) {
      cout << nodes_on_lvl << endl;
      last_lvl = lvl;
      nodes_on_lvl = 0;
    } else {
      nodes_on_lvl++;
    }

    cout << GetNodeTypeName(n->type()) << " (" << lvl << ")" << dup(' ', 10);
  }

  cout << endl;
}
