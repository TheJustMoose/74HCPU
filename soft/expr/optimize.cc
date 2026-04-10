#include "optimize.h"

#include <iostream>

using namespace std;

void Optimize(vector<Operation>& res_code) {
  vector<size_t> can_be_optimized {};

  if (res_code.size() <= 1)
    return;

  // t0 = 1  (res_in_temp == true)
  // x = t0  (res_in_temp == false)
  // t0 is temporary variable, so we can remove second line of code and patch first line
  // result: x = 1

  for (size_t i = 0; i < res_code.size() - 1; i++) {
    if (res_code[i].res_in_temp && ! res_code[i + 1].res_in_temp &&
        res_code[i].res_arg == res_code[i + 1].left_arg) {

      cout << "Will merge " << i << " with " << i + 1 << " operations" << endl;
      cout << "i   " << res_code[i].str() << endl;
      cout << "i+1 " << res_code[i + 1].str() << endl;

      can_be_optimized.push_back(i + 1);
      //cout << res_code[i].str() << " - can be optimized! i: " << i << endl;
      res_code[i].res_arg = res_code[i + 1].res_arg;
      res_code[i].res_in_temp = false;  // see first condition in if
      res_code[i + 1].clear();
      i++;  // have to skip just cleared instruction
    }
  }

  if (can_be_optimized.empty()) {
    cout << "Nothing to optimize" << endl;
    return;
  }

  cout << "can_be_optimized.size(): " << can_be_optimized.size() << endl;

  // now we can remove all empty ( .clear() ) strings
  for (auto it = can_be_optimized.rbegin(); it != can_be_optimized.rend(); it++) {
    //cout << "Line with idx == " << *it << " will be removed"
    //     << "  " << res_code[*it].str() << endl;
    res_code.erase(res_code.begin() + *it);
  }
}
