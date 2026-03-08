#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "func_guard.h"
#include "lexer.h"
#include "node.h"
#include "optimize.h"
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

  if (argc == 3 && argv[1] && argv[2]) {
    string key(argv[1]);
    if (key == "-f") {
      string fname(argv[2]);
      ifstream f(fname, ios_base::in);
      if (!f) {
        cout << "Can't open input file " << fname << endl;
        return 1;
      }

      stringstream buf;
      buf << f.rdbuf();
      string content = buf.str();
      Lexer::instance().setInputString(content);
    }
  } else if (argc == 2 && argv[1]) {
    cout << "Try to process: \"" << argv[1] << "\"" << endl;
    string s {argv[1]};
    Lexer::instance().setInputString(s);
  } else {
    cout << "Using: plus.exe \"1+2+3\"" << endl;
    return 1;
  }

  if (!stmt(statements))
    return 1;

  cout << "nodes:" << endl;
  vector<Operation> res_code;
  for (Node* n : statements) {
    n->gen(res_code);  // enum tree items, generate three-address code
    cout << "expr res: " << n->res() << endl;
  }

  cout << "res_code.size(): " << res_code.size() << endl;

  Optimize(res_code);

  cout << "final asm:" << endl;
  for (Operation& n : res_code) {
    cout << n.str() << endl;
    //cout << "args: " << n.argNum() << endl;
  }

  cout << "main finished" << endl;
  return 0;
}
