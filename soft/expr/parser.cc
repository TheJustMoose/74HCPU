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
// --------------------------------------------
//  C-like language grammar
//  - global variable declarations only at top, then functions
//  - pointers: declared with '@' (e.g., int @p)
//  - dereference: '@' (unary) in expressions
//  - address-of: '#' only for variable names
//  - post-increment '++' allowed only on variable names
//    and on dereferenced pointer variables (@name++)
//  - all logical, bitwise, relational, arithmetic operators
//  - statements: assignment, while, if, block, return
// --------------------------------------------

Primary             ::= num
                    |   name [ '++' ]
                    |   '-' Primary
                    |   '@' name [ '++' ]   // dereference then increment pointer
                    |   '#' name            // address-of, no increment allowed
                    |   '(' Expression ')'
                    ;

Term                ::= Primary { ('*' | '/') Primary };

AdditiveExpr        ::= Term { ('+' | '-') Term };

RelationalExpr      ::= AdditiveExpr { ('<' | '>' | '<=' | '>=') AdditiveExpr };

EqualityExpr        ::= RelationalExpr { ('==' | '!=') RelationalExpr };

BitwiseAndExpr      ::= EqualityExpr { '&' EqualityExpr };

BitwiseOrExpr       ::= BitwiseAndExpr { '|' BitwiseAndExpr };

LogicalAndExpr      ::= BitwiseOrExpr { '&&' BitwiseOrExpr };

LogicalOrExpr       ::= LogicalAndExpr { '||' LogicalAndExpr };

// Expression hierarchy (precedence from low to high)
Expression          ::= LogicalOrExpr;

// Assignment (right-associative, as in C)
Assignment          ::= Expression [ '=' Assignment ];

// List of variable names, each optionally initialized with a number
InitDeclaratorList  ::= name [ '=' num ] { ',' name [ '=' num ] };

// Declaration: type, optional '@', then a list of variables with optional init
Declaration         ::= Type [ '@' ] InitDeclaratorList;
Type                ::= 'int'  |  'byte';

IfStatement         ::= 'if' '(' Expression ')' Statement [ 'else' Statement ];

WhileStatement      ::= 'while' '(' Expression ')' Statement;

ReturnStatement     ::= 'return' Expression ';'
                    |   'return' ';'
                    ;

// Statements: assignment, while, if, block, return
Statement           ::= Assignment ';'
                    |   WhileStatement
                    |   IfStatement
                    |   Block
                    |   ReturnStatement
                    ;

// Block: declarations and statements mixed (C99 style)
Block               ::= '{' { Declaration ';' | Statement } '}';

// Parameter list: possibly empty, separated by commas
ParameterList       ::= [ Parameter { ',' Parameter } ];
Parameter           ::= Type name;

// Return type: either a normal Type or 'none'
ReturnType          ::= Type  |  'none' ;

// Function definition: return type (Type or 'none'), name, parameters, block
FunctionDefinition  ::= ReturnType name '(' ParameterList ')' Block;

FunctionDefinitions ::= { FunctionDefinition }; // zero or more functions

GlobalDeclarations  ::= { Declaration ';' };   // zero or more global variable declarations

// Program: global variable declarations (only at the top)
// followed by function definitions.
Program             ::= GlobalDeclarations FunctionDefinitions;

*/

int tmp_var_counter {0};
string new_tmp() {
  return string("t") + to_string(tmp_var_counter++);
}

unique_ptr<Node> prim_num() {
  int val = Lexer::instance().getIntValue();
  Lexer::instance().consume();
  return make_unique<Num>(val);
}

unique_ptr<Node> prim_name() {
  string str = Lexer::instance().getStrValue();
  Lexer::instance().consume();
  unique_ptr<Name> nm = make_unique<Name>(str);
  Token t = Lexer::instance().currentToken();
  if (t == tIncrement) {
    Lexer::instance().consume();
    return make_unique<IncrementOp>(std::move(nm));
  }
  return nm;
}

unique_ptr<Node> prim_minus() {
  Lexer::instance().consume();
  unique_ptr<UnMinus> n = make_unique<UnMinus>(new_tmp());
  n->child = prim();
  if (!n->child)
    throw logic_error("Argument for unary minus was not found");
  return n;
}

unique_ptr<Node> prim_atsign() {
  Lexer::instance().consume();
  Token t = Lexer::instance().currentToken();
  if (t != tName)
    throw logic_error("Error. Waiting for name. You can dereference only variable");
  return make_unique<DereferenceOp>(prim());
}

unique_ptr<Node> prim_numbersign() {
  Lexer::instance().consume();
  Token t = Lexer::instance().currentToken();
  if (t != tName)
    throw logic_error("Error. Waiting for name. You can get only variable address");
  return make_unique<AddressOf>(prim());
}

unique_ptr<Node> prim_lbracket() {
  Lexer::instance().consume();
  unique_ptr<Node> n = expr();
  Token t = Lexer::instance().currentToken();
  if (t != tRBracket)
    throw logic_error("Error. Waiting for right bracket");
  Lexer::instance().consume();
  return n;
}

unique_ptr<Node> prim() {
  FuncGuard fg("prim");
  Token t = Lexer::instance().currentToken();
  if (t == tNum) {
    return prim_num();
  } else if (t == tName) {
    return prim_name();
  } else if (t == tMinus) {
    return prim_minus();
  } else if (t == tAtSign) {
    return prim_atsign();
  } else if (t == tNumberSign) {
    return prim_numbersign();
  } else if (t == tLBracket) {
    return prim_lbracket();
  } else {
    return {};
  }
}

// Term ::= Primary { ('*' | '/') Primary };
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

// AdditiveExpr ::= Term { ('+' | '-') Term };
unique_ptr<Node> additive_expr() {
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

// RelationalExpr ::= AdditiveExpr { ('<' | '>' | '<=' | '>=') AdditiveExpr };
unique_ptr<Node> relational_expr() {
  unique_ptr<Node> left = additive_expr();
  if (!left)
    return {};

  Token t = Lexer::instance().currentToken();
  // Comparison operations, unlike arithmetic operations, usually contain only two operands.
  if (t == tLess || t == tGreater || t == tLessOrEqual || t == tGreaterOrEqual) {
    Lexer::instance().consume();

    unique_ptr<Node> right = additive_expr();

    unique_ptr<RelationalOp> op = make_unique<RelationalOp>(t, new_tmp());
    op->left = std::move(left);
    op->right = std::move(right);
    return op;
  }

  return left;
}

// EqualityExpr ::= RelationalExpr { ('==' | '!=') RelationalExpr };
unique_ptr<Node> equality_expr() {
  return relational_expr();
}

// BitwiseAndExpr ::= EqualityExpr { '&' EqualityExpr };
unique_ptr<Node> bitwise_and_expr() {
  return equality_expr();
}

// BitwiseOrExpr ::= BitwiseAndExpr { '|' BitwiseAndExpr };
unique_ptr<Node> bitwise_or_expr() {
  return bitwise_and_expr();
}

// LogicalAndExpr ::= BitwiseOrExpr { '&&' BitwiseOrExpr };
unique_ptr<Node> logical_and_expr() {
  return bitwise_or_expr();
}

// LogicalOrExpr ::= LogicalAndExpr { '||' LogicalAndExpr };
unique_ptr<Node> logical_or_expr() {
  return logical_and_expr();
}

// Expression ::= LogicalOrExpr;
unique_ptr<Node> expr() {
  return logical_or_expr();
}

// Assignment ::= Expression [ '=' Assignment ];
unique_ptr<Node> assign() {
  FuncGuard fg("assi");
  unique_ptr<Node> left = expr();  // a
  if (!left)
    return {};

  uint8_t var_size {0};
  if (left->node_type() == ntName) {
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
        n->values[var_name] = val;
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
