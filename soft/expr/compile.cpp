#include "compile.h"
#include "backend.h"
#include "func_guard.h"
#include "lexer.h"
#include "node.h"
#include "nodes.h"
#include "optimize.h"
#include "parser.h"
#include "print_tree.h"
#include "var_list.h"

#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

using namespace std;

void Print(const vector<Operation>& res_code) {
  cout << "| res | = | left|  op |right|isNum|" << endl;
  for (const Operation& r : res_code)
    cout << r.raw() << endl;
}

void PrintIR(const vector<Operation>& res_code) {
  for (const Operation& n : res_code) {
    uint8_t sz {0};
    if (isDeclared(n.res_arg, &sz))
      cout << n.str() << "(size: " << static_cast<int>(sz) << ")" << endl;
    else if (n.res_in_temp)
      cout << n.str() << endl;
    else
      cout << n.str() << "  // was not declared" << endl;
  }
}

void PrintLiveVars(const vector<Operation>& res_code,
                   map<size_t, string> idx_to_var) {
  cout << "| res | = | left|  op |right|isNum|" << endl;
  for (const Operation& r : res_code) {
    cout << r.raw() << endl;
    cout << r.live_vars_str(idx_to_var) << endl;
  }
}

int compile(string code) {
  FuncGuard fg("compile");

  Lexer::instance().setInputString(code);

  vector<unique_ptr<Node>> statements;
  if (!stmts(statements))
    return 1;

  for (size_t i = 0; i < statements.size(); i++) {
    VarDecl* decl_node = dynamic_cast<VarDecl*>(statements[i].get());
    if (!decl_node)
      continue;

    for (string var_name : decl_node->names) {
      AddVar(var_name, decl_node->data_type(), decl_node->is_pointer());
      cout << GetDataTypeName(decl_node->data_type())
           << " variable \"" << var_name
           << (decl_node->is_pointer() ? "@" : "")
           << "\" has been declared" << endl;
    }
  }

  map<string, uint16_t> var_addrs;
  map<string, size_t> var_idxs;
  map<size_t, string> idx_to_var;

  cout << "decls:" << endl;
  uint16_t addr {0};
  cout << "| var | data_type | addr |" << endl;
  for (size_t i = 0; i < getVarCount(); i++) {
    Var v = getVar(i);
    cout << "| " << v.name << " | "
         << GetDataTypeName(v.data_type) << (v.is_ptr ? "@" : "")
         << " | " << addr << " | " << endl;

    var_addrs[v.name] = addr;  // var name -> var addr
    var_idxs[v.name] = i;      // index of variable inside of parser
    idx_to_var[i] = v.name;    // GetVar duplicate
    addr += v.size();
  }

  cout << "nodes:" << endl;
  vector<Operation> res_code;
  for (size_t i = 0; i < statements.size(); i++)
    if (statements[i])
      statements[i]->gen(res_code);  // enum tree items, generate three-address code
    else
      cout << "statements[i] is null" << endl;

  cout << "res_code.size(): " << res_code.size() << endl << endl;

  for (size_t i = 0; i < statements.size(); i++)
    if (statements[i])
      PrintTree(statements[i].get());

  Print(res_code);
  cout << endl;

  cout << "try to optimize:" << endl;
  Optimize(res_code);
  cout << "res_code.size(): " << res_code.size() << endl;

  Print(res_code);

  cout << endl << "final IR:" << endl;
  PrintIR(res_code);

  cout << endl << "vars:" << endl;
  printVars();

  cout << endl << "processing live&dead vars..." << endl;
  vector<bool> live_vars(var_idxs.size(), false);
  for (int i = res_code.size() - 1; i >= 0; i--) {
    Operation& op = res_code[i];
    op.live_out_vars = live_vars;

    auto mark_var = [var_idxs, &live_vars](string name, bool mark) {
      // map<string, size_t> var_idxs;
      auto res_it = var_idxs.find(name);
      if (res_it != var_idxs.end())
        live_vars[res_it->second] = mark;
    };

    // -res, +left, +right
    mark_var(op.res_arg, false);
    mark_var(op.left_arg, true);
    mark_var(op.right_arg, true);

    op.live_in_vars = live_vars;
  }

  cout << endl << "printing live&dead vars..." << endl;
  PrintLiveVars(res_code, idx_to_var);
  cout << "finished" << endl << endl;

  Backend bcknd(var_addrs, idx_to_var);
  bcknd.GenerateCode(res_code);

  cout << endl << "final asm:" << endl;

  vector<string> res_asm = bcknd.GetResAsm();
  for (string& s : res_asm)
    cout << s << endl;

  return 0;
}
