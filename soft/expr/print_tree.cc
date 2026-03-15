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
map<const Node*, int> gNodeOffset;

void Tree2List(Node* n, int mid, int lvl = 0) {
  gLinearTree.push_back(n);
  gNodeLevel[n] = lvl;
  gNodeOffset[n] = mid;

  int offset = mid/4;
  if (BinOp* bop = dynamic_cast<BinOp*>(n)) {
    Tree2List(bop->left, mid - mid/2 - offset, lvl + 1);
    Tree2List(bop->right, mid - mid/2 + offset, lvl + 1);
  } else if (AssignOp* aop = dynamic_cast<AssignOp*>(n)) {
    Tree2List(aop->left, mid + mid/2 - offset, lvl + 1);
    Tree2List(aop->right, mid + mid/2 - offset, lvl + 1);
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

  Tree2List(n, kWidth / 2);  // root node should be in a center of screen
  cout << endl;

  char line[kWidth + 1];

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

    cout << dup(' ', w) << GetNodeTypeName(n->type()) << " (" << gNodeOffset[n] << ")" << dup(' ', 10);
  }

  cout << endl;
  cout << dup('-', kWidth) << endl;
}
