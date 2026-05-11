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

#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// Prototype to play with tree
//

/*
Program         ::= Statement { ';' Statement } [ ';' ]
                |   ε
                ;

Statement       ::= Declaration
                |   Assignment
                ;

Declaration     ::= Type [ '@' ] NameList
                ;

Type            ::= 'int'
                |   'byte'
                ;

NameList        ::= name [ '=' num ] { ',' name [ '=' num ] }
                ;

Assignment      ::= Expression [ '=' Assignment ]
                ;

Expression      ::= Term { ('+' | '-') Term }
                ;

Term            ::= Primary { ('*' | '/') Primary }
                ;

Primary         ::= num
                |   name
                |   '-' Primary
                |   '(' Expression ')'
                ;
*/

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

unique_ptr<Node> prim() {
  FuncGuard fg("prim");
  Token t = Lexer::instance().currentToken();
  if (t == tNum) {
    int val = Lexer::instance().getIntValue();
    Lexer::instance().consume();
    return make_unique<Num>(val);
  } else if (t == tName) {
    string str = Lexer::instance().getStrValue();
    Lexer::instance().consume();
    return make_unique<Name>(str);
/*
    DataType dt {dtNotInitialize};
    if (isDeclared(str, nullptr, &dt))  // вынести на потом
      return make_unique<Name>(str, dt);
    else
      return make_unique<Name>(str);
*/
  } else if (t == tMinus) {
    Lexer::instance().consume();
    unique_ptr<UnOp> n = make_unique<UnOp>(new_tmp());
    n->child = prim();
    if (!n->child)
      throw logic_error("Argument for unary minus was not found");
    return n;
  } else if (t == tLBracket) {
    Lexer::instance().consume();
    unique_ptr<Node> n = expr();
    t = Lexer::instance().currentToken();
    if (t != tRBracket)
      cout << FuncGuard::stack_str()
           << "Error. Waiting for right bracket" << endl;
    else
      Lexer::instance().consume();
    return n;
  } else {
    return {};
  }
}

unique_ptr<Node> term() {
  FuncGuard fg("term");
  unique_ptr<Node> left = prim();
  if (!left)
    return {};

  Token t = Lexer::instance().currentToken();
  while (t == tMul || t == tDiv) {
    Token t_op = t;
    Lexer::instance().consume();

    unique_ptr<Node> right = prim();
    // prim has already called Lexer::instance().consume();

    unique_ptr<BinOp> op = make_unique<BinOp>(t_op, new_tmp());
    op->left = std::move(left);
    op->right = std::move(right);
    left = std::move(op);
    t = Lexer::instance().currentToken();
  }

  return left;
}

unique_ptr<Node> expr() {
  FuncGuard fg("expr");
  unique_ptr<Node> left = term();
  if (!left)
    return {};

  Token t = Lexer::instance().currentToken();
  while (t == tPlus || t == tMinus) {
    Token t_op = t;
    Lexer::instance().consume();

    unique_ptr<Node> right = term();
    // term has already called Lexer::instance().consume(); in prim()

    unique_ptr<BinOp> op = make_unique<BinOp>(t_op, new_tmp());
    op->left = std::move(left);
    op->right = std::move(right);
    left = std::move(op);
    t = Lexer::instance().currentToken();
  }

  return left;
}

unique_ptr<Node> assign() {
  FuncGuard fg("assi");
  unique_ptr<Node> left = expr();  // a
  if (!left)
    return {};

  uint8_t var_size {0};
  if (left->type() == ntName) {
    Name* n = dynamic_cast<Name*>(left.get());
    // вообще, где-то здесь не хватает проверки res_in_temp
    // но этот флажок лежит в таблице со списком операций :(
    if (!n)
      throw logic_error("dynamic_cast<Name*> return NULL");
/*
    // вынести на потом:
    if (!isDeclared(n->name(), &var_size))
      cout << "You try to assign to variable \"" << n->name()
           << "\" which has not been declared" << endl;
*/
  }

  // TODO: теперь хорошо бы добавить инициализацию объявляемых переменных
  // и сделать оператор взятия адреса - & (или @ ?)

  Token t = Lexer::instance().currentToken();
  if (t == tEnd)
    return left;

  unique_ptr<Node> res;
  if (t == tEqual) {
    Lexer::instance().consume();
    unique_ptr<AssignOp> op = make_unique<AssignOp>();  // =
    op->left = std::move(left);     // a =
    op->right = assign();           // a = assign(); // recursion
    res = std::move(op);    // remove res, use return op
  } else {
    res = std::move(left);  // remove res, use return left
  }

  return res;
}

unique_ptr<Node> declare() {
  FuncGuard fg("decl");

  Token t = Lexer::instance().currentToken();
  if (t == tEnd)
    return {};

  DataType dt {dtNotInitialize};
  if (t == tInt) {
    dt = dtInt;
  } else if (t == tByte) {
    dt = dtByte;
  } else {
    return {};
  }

  Lexer::instance().consume();  // skip int

  bool is_ptr = false;
  t = Lexer::instance().currentToken();
  if (t == tAtSign) {  // pointer sign - @ (not asterisk)
    is_ptr = true;
    Lexer::instance().consume();
  }

  unique_ptr<VarDecl> n = make_unique<VarDecl>(dt, is_ptr);

  // Okay, now try to find variable name[s]
  t = Lexer::instance().currentToken();
  while (t == tName) {
    string var_name = Lexer::instance().getStrValue();
    n->names.push_back(var_name);

    Lexer::instance().consume();  // skip var name

    t = Lexer::instance().currentToken();
    if (t == tEqual) {
      Lexer::instance().consume();
      t = Lexer::instance().currentToken();
      if (t == tNum) {
        int val = Lexer::instance().getIntValue();
        Lexer::instance().consume();
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
    } else if (t == tSemicolon) {
      return n;
    } else {
      cout << "Error. Please add ';' to the end of declaration" << endl;
      cout << "Got token: " << (int)t << endl;
      return {};
    }
  }

  return {};
}

unique_ptr<Node> stmt() {
  // stmt -> assign | declare
  unique_ptr<Node> n = declare();
  if (!n)
    n = std::move(assign());
  return n;
}

bool stmts(vector<unique_ptr<Node>>& statements) {
  FuncGuard fg("stmt");

  unique_ptr<Node> n(stmt());
  Token t = Lexer::instance().currentToken();
  while (n) {
    if (t == tSemicolon) {
      if (!n) {
        cout << "Node == null was returned from stmt()" << endl;
        break;
      }

      statements.push_back(std::move(n));

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
