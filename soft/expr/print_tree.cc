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

const int kWidth = 100;

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
  //cout << "mid: " << mid << ", lvl: " << lvl << endl;

  if (BinOp* bop = dynamic_cast<BinOp*>(n)) {
    //cout << "binop..." << endl;
    lvl++;
    int offset = kWidth / (lvl*2 + 5);
    Tree2List(bop->left, mid - offset, lvl);
    Tree2List(bop->right, mid + offset, lvl);
  } else if (AssignOp* aop = dynamic_cast<AssignOp*>(n)) {
    //cout << "assign..." << endl;
    lvl++;
    int offset = kWidth / (lvl*2 + 5);
    Tree2List(aop->left, mid - offset, lvl);
    Tree2List(aop->right, mid + offset, lvl);
  } else if (UnOp* uop = dynamic_cast<UnOp*>(n)) {
    //cout << "unary minus..." << endl;
    lvl++;
    Tree2List(uop->child, mid, lvl);
  } else {
    //cout << "Node: " << GetNodeTypeName(n->type()) << endl;
  }
}

void TypeStringIntoTheBuffer(string& line, string word, int pos) {
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

  gLinearTree.clear();
  gNodeLevel.clear();

  Tree2List(n, kWidth / 2);  // root node should be in a center of screen
  //cout << gLinearTree.size() << " items was stored" << endl;

  string line(kWidth, ' ');
  int last_lvl {0};
  int nodes_on_lvl {0};
  int cnt = 0;
  for (size_t i = 0; i < gLinearTree.size(); i++) {
    const Node* n = gLinearTree[i];
    int lvl = gNodeLevel[n];
    if (last_lvl != lvl) {
      cout << line << endl;
      //cout << "The next level..." << endl;
      line = string(kWidth, ' ');
      last_lvl = lvl;
      nodes_on_lvl = 1;  // we just found node on next level and node exist, so 'cnt' = 1
    } else {
      nodes_on_lvl++;
    }

    TypeStringIntoTheBuffer(
        line,
        GetNodeTypeName(n->type()) + " (" + to_string(gNodeOffset[n]) + ")",
        gNodeOffset[n]);

    //cout << "cnt: " << cnt << ", " << GetNodeTypeName(n->type()) << endl;
    cnt++;
  }

  // Do not lose last iteration of loop!
  if (nodes_on_lvl > 0)
    cout << line << endl;

  cout << endl << dup('-', kWidth) << endl;
}
