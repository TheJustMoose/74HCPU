#include "parser.h"

#include "data_type.h"
#include "func_guard.h"
#include "lexer.h"
#include "names.h"
#include "node.h"
#include "nodes.h"
#include "node_type.h"
#include "operation.h"
#include "token.h"
#include "var.h"
#include "var_size.h"

#include <assert.h>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

// Prototype to play with tree
//

/*
name -> id (a few letters)
prim -> num | name | (expr)
term -> term * prim
expr -> expr + term
assi -> assi = expr | name


term_rest -> * prim | E
term -> prim | term_rest

*/

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

vector<Var> vars {};

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

Node* prim() {
  FuncGuard fg("prim");
  Token t = Lexer::instance().currentToken();
  if (t == tNum) {
    int val = Lexer::instance().getIntValue();
    Lexer::instance().consume();
    return new Num(val);
  } else if (t == tName) {
    string str = Lexer::instance().getStrValue();
    Lexer::instance().consume();

    DataType dt {dtNotInitialize};
    if (isDeclared(str, nullptr, &dt))
      return new Name(str, dt);
    else
      return new Name(str);
  } else if (t == tMinus) {
    Lexer::instance().consume();
    UnOp* n = new UnOp(new_tmp());
    n->child = prim();
    assert(n->child);
    vars.emplace_back(n->name(), n->data_type(), false); // is_ptr ?
    return n;
  } else if (t == tLBracket) {
    Lexer::instance().consume();
    Node* n = expr();
    t = Lexer::instance().currentToken();
    if (t != tRBracket)
      cout << FuncGuard::stack_str()
           << "Error. Waiting for right bracket" << endl;
    else
      Lexer::instance().consume();
    return n;
  } else {
    return nullptr;
  }
}

Node* term() {
  FuncGuard fg("term");
  Node* left = prim();
  if (!left)
    return nullptr;

  Token t = Lexer::instance().currentToken();
  while (t == tMul || t == tDiv) {
    Token t_op = t;
    Lexer::instance().consume();

    Node* right = prim();
    // prim have already called Lexer::instance().consume();

    BinOp* op = new BinOp(t_op, new_tmp());
    op->left = left;
    op->right = right;
    left = op;
    vars.emplace_back(op->name(), op->data_type(), false); // is_ptr ?
    t = Lexer::instance().currentToken();
  }

  return left;
}

Node* expr() {
  FuncGuard fg("expr");
  Node* left = term();
  if (!left) {
    return nullptr;
  }

  Token t = Lexer::instance().currentToken();
  while (t == tPlus || t == tMinus) {
    Token t_op = t;
    Lexer::instance().consume();

    Node* right = term();
    // term have already called Lexer::instance().consume(); in prim()

    BinOp* op = new BinOp(t_op, new_tmp());
    op->left = left;
    op->right = right;
    left = op;
    vars.emplace_back(op->name(), op->data_type(), false); // is_ptr ?
    t = Lexer::instance().currentToken();
  }

  return left;
}

Node* assign() {
  FuncGuard fg("assi");
  Node* left = expr();  // a
  if (!left)
    return nullptr;

  uint8_t var_size {0};
  if (left->type() == ntName) {
    Name* n = dynamic_cast<Name*>(left);
    // вообще, где-то здесь не хватает проверки res_in_temp
    // но этот флажок лежит в таблице со списком операций :(
    if (!isDeclared(n->name(), &var_size))
      cout << "You try to assign to variable \"" << n->name()
           << "\" which has not been declared" << endl;
  }

  // TODO: теперь хорошо бы добавить инициализацию объявляемых переменных
  // и сделать оператор взятия адреса - & (или @ ?)

  Token t = Lexer::instance().currentToken();
  if (t == tEnd)
    return left;

  Node* res {nullptr};
  if (t == tEqual) {
    Lexer::instance().consume();
    AssignOp* op = new AssignOp();  // =
    op->left = left;                // a =
    op->right = assign();           // a = assign(); // recursion
    res = op;
  } else {
    res = left;
  }

  return res;
}

Node* declare() {
  FuncGuard fg("decl");

  Token t = Lexer::instance().currentToken();
  if (t == tEnd)
    return nullptr;

  DataType dt {dtNotInitialize};
  if (t == tInt) {
    dt = dtInt;
  } else if (t == tByte) {
    dt = dtByte;
  } else {
    return nullptr;
  }

  Lexer::instance().consume();  // skip int

  bool is_ptr = false;
  t = Lexer::instance().currentToken();
  if (t == tAtSign) {  // pointer sign - @ (not asterisk)
    is_ptr = true;
    Lexer::instance().consume();
  }

  VarDecl* n = new VarDecl(dt, is_ptr);

  // Okay, now try to find variable name[s]
  t = Lexer::instance().currentToken();
  while (t == tName) {
    string var_name = Lexer::instance().getStrValue();
    n->names.push_back(var_name);
    cout << "Int variable \"" << var_name << "\" has been declared" << endl;

    vars.emplace_back(var_name, dt, is_ptr);  // duplicate variable name array
    Lexer::instance().consume();  // skip var name

    t = Lexer::instance().currentToken();
    if (t == tEqual) {
      Lexer::instance().consume();
      t = Lexer::instance().currentToken();
      if (t == tNum) {
        Lexer::instance().consume();
        int val = Lexer::instance().getIntValue();
        cout << var_name << " := " << val << "  // initialization" << endl;
        // TODO: try to put var value into **vars** vector (see above)
      } else {
        cout << "Error. Only initialization by numbers is supported now." << endl;
      }
    }

    t = Lexer::instance().currentToken();
    if (t == tComma) {
      Lexer::instance().consume();  // skip comma
      t = Lexer::instance().currentToken();
    } else if (t == tSemicolon) {  // replace it to comma later
      return n;
    } else {
      cout << "Error. Please add ';' to the end of declaration" << endl;
      cout << "Got token: " << (int)t << endl;
      return nullptr;
    }
  }

  return nullptr;
}

Node* stmt() {
  // stmt -> assign | declare
  Node* n = declare();
  return n ? n : assign();
}

bool stmts(vector<Node*>& statements) {
  FuncGuard fg("stmt");

  Node* n = stmt();
  Token t = Lexer::instance().currentToken();
  while (n) {
    if (t == tSemicolon) {
      if (!n) {
        cout << "Node == null was returned from stmt()" << endl;
        break;
      }
      statements.push_back(n);

      Lexer::instance().consume();
      n = stmt();
      t = Lexer::instance().currentToken();
    } else {
      cout << FuncGuard::stack_str()
           << "**** Error. Please add ';' to the end of statement ****" << endl;
      return false;
    }
  }

  return true;
}
