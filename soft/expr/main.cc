#include "compile.h"
#include "func_guard.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  FuncGuard fg("main");
  cout << "stack: " << FuncGuard::stack_str() << endl;
  cout << "argc: " << argc << endl;
  for (int i = 0; i < argc; i++)
    cout << "i: " << i << ", arg[i]: " << argv[i] << endl;
  cout << endl;

  int res {0};
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
      try {
        Compile(buf.str());
      } catch (const std::exception& e) {
        cout << "Compile error: " << e.what() << endl;
      }
    }
  } else if (argc == 2 && argv[1]) {
    cout << "Try to process: \"" << argv[1] << "\"" << endl;
    try {
      res = Compile(argv[1]);
    } catch (const std::exception& e) {
      cout << "Compile error: " << e.what() << endl;
    }
  } else {
    cout << "Using: simple_expr.exe \"1+2+3;\"" << endl;
    return 1;
  }

  cout << "main finished" << endl;
  return res;
}
