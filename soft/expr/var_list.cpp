#include "var_list.h"

#include <iostream>
#include <vector>

#include "data_type.h"

using namespace std;

vector<Var> vars {};

bool AddVar(std::string var_name, DataType dt, bool is_ptr) {
  for (const Var& v : vars)
    if (v.name == var_name)
      return false;

  vars.emplace_back(var_name, dt, is_ptr);
  return true;
}

size_t getVarCount() {
  return vars.size();
}

Var getVar(size_t idx) {
  if (idx < vars.size())
    return vars[idx];
  return {};
}

bool isDeclared(string var_name, uint8_t* var_size, DataType* dt) {
  for (const Var& v : vars)
    if (v.name == var_name) {
      if (var_size != nullptr) {
        if (v.size() == 0)
          cout << "Error! var_size return 0!" << endl;
        else
          *var_size = v.size();
      }
      if (dt)
        *dt = v.data_type;
      return true;
    }

  return false;
}

void printVars() {
  for (const Var& v : vars)
    cout << v.name << ": " << GetDataTypeName(v.data_type)
         << (v.is_ptr ? "@" : "")
         << ", size: " << (int)v.size() << endl;
}

bool getVar(string var_name, Var& var) {
  for (const Var& v : vars)
    if (v.name == var_name) {
      var = v;
      return true;
    }

  return false;
}
