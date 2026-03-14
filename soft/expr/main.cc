#include "assemble.h"
#include "func_guard.h"
#include "lexer.h"
#include "node.h"
#include "optimize.h"
#include "parser.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

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

  vector<Node*> statements;
  if (!stmts(statements))
    return 1;

  cout << "nodes:" << endl;
  vector<Operation> res_code;
  for (size_t i = 0; i < statements.size(); i++)
    if (statements[i])
      statements[i]->gen(res_code);  // enum tree items, generate three-address code
    else
      cout << "statements[i] is null" << endl;

  cout << "res_code.size(): " << res_code.size() << endl << endl;

/*
  std::string res_arg {};
  std::string op_name {};
  std::string left_arg {};
  std::string right_arg {};
  bool res_in_temp {false};
*/

  cout << "| res |  op | left|right| tmp |" << endl;
  for (auto& r : res_code)
    cout << r.raw() << endl;

  cout << endl;

  cout << "try to optimize:" << endl;
  Optimize(res_code);
  cout << "res_code.size(): " << res_code.size() << endl;

  cout << endl << "final IR:" << endl;
  for (Operation& n : res_code) {
    uint8_t sz {0};
    if (isDeclared(n.res_arg, &sz))
      cout << n.str() << "(" << static_cast<int>(sz) << ")" << endl;
    else if (n.res_in_temp)
      cout << n.str() << endl;
    else
      cout << n.str() << "  // was not declared" << endl;
  }

  cout << endl << "final asm:" << endl;
  vector<string> res_asm = Assemble(res_code);
  for (string& s : res_asm)
    cout << s << endl;

  cout << "main finished" << endl;
  return 0;
}
