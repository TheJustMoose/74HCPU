#include "func_guard.h"

using namespace std;

//static
stack<string> FuncGuard::call_stack_ {};

//static
string FuncGuard::stack_str() {
  stack<string> s = call_stack_;
  string res;
  while (!s.empty()) {
    res = s.top() + string(">") + res;
    s.pop();
  }
  res += " ";
  return res;
}
