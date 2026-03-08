#include <iostream>
#include <vector>

#include "func_guard.h"
#include "lexer.h"
#include "node.h"
#include "parser.h"

using namespace std;

vector<Node*> statements;

int main(int argc, char* argv[]) {
  FuncGuard fg("main");
  cout << "stack: " << FuncGuard::stack_str() << endl;
  cout << "argc: " << argc << endl;
  for (int i = 0; i < argc; i++) {
    cout << "i: " << i << ", arg[i]: " << argv[i] << endl;
  }
  cout << endl;

  if (argc == 2 && argv[1]) {
    cout << "Try to process: \"" << argv[1] << "\"" << endl;
    string s {argv[1]};
    Lexer::instance().setInputString(s);
  } else {
    cout << "Using: plus.exe \"1+2+3\"" << endl;
    return 1;
  }

  if (!stmt(statements))
    return 1;

  for (Node* n : statements) {
    n->gen();  // enum tree items, generate three-address code 
    cout << "expr res: " << n->res() << endl;
  }

  cout << "main finished" << endl;
  return 0;
}
