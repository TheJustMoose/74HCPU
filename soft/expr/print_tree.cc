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

  if (BinOp* bop = dynamic_cast<BinOp*>(n)) {
    Tree2List(bop->left, mid - (lvl + 1)*5, lvl + 1);
    Tree2List(bop->right, mid + (lvl + 1)*5, lvl + 1);
  } else if (AssignOp* aop = dynamic_cast<AssignOp*>(n)) {
    Tree2List(aop->left, mid - (lvl + 1)*5, lvl + 1);
    Tree2List(aop->right, mid + (lvl + 1)*5, lvl + 1);
  } else {
    cout << "Node: " << GetNodeTypeName(n->type()) << endl;
  }
}

void TypeInTheStringIntoBuffer(string& line, string word, int pos) {
  int left = pos - word.size() / 2;
  if (left < 0 || left >= line.size()) {
    cout << "Error! Word position is ouf of bounds!!" << endl;
    return;
  }

  for (size_t i = 0; i < word.size(); i++) {
    if (left + i >= line.size())
      break;
    line[left + i] = word[i];
  }
}

void PrintTree(Node* n) {
  if (!n)
    return;

  //cout << dup(' ', width / 2) << n->op() << endl;
  gLinearTree.clear();
  gNodeLevel.clear();

  Tree2List(n, kWidth / 2);  // root node should be in a center of screen
  cout << gLinearTree.size() << " items was stored" << endl;

  string line(80, ' ');
  if (line.size() != 80)
    cout << "Error!!! line.size() != 80. It's " << line.size() << endl;

  int last_lvl {0};
  int nodes_on_lvl {0};
  int w = kWidth / 2;
  int cnt = 0;
  for (const Node* n: gLinearTree) {
    cout << "cnt: " << cnt++ << endl;
    int lvl = gNodeLevel[n];
    if (last_lvl != lvl) {
      //cout << "n on lvl: " << nodes_on_lvl << endl;
      cout << line << endl;
      line = string(80, ' ');
      last_lvl = lvl;
      nodes_on_lvl = 0;
      w /= 2;
    } else {
      nodes_on_lvl++;
    }

    TypeInTheStringIntoBuffer(line, GetNodeTypeName(n->type()) + to_string(gNodeOffset[n]), gNodeOffset[n]);
  }

  cout << endl;
  cout << dup('-', kWidth) << endl;
}
