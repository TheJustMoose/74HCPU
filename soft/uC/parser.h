#pragma once

#include "tag.h"

#include <memory>
#include <string>

class Access;
class Env;
class Expr;
class Id;
class Lexer;
class Stmt;
class Token;
class Type;

class Parser {
 public:
  Parser(Lexer* l);

  void pmove();
  void error(std::string s);
  void match(Tag t);
  void match(char c);
  void program();
  Stmt* block();
  void decls();
  Type* type();
  Type* dims(Type* p);
  Stmt* stmts();
  Stmt* stmt();
  Stmt* assign();
  Expr* boolean();
  Expr* join();
  Expr* equality();
  Expr* rel();
  Expr* expr();
  Expr* term();
  Expr* unary();
  Expr* factor();
  Access* offset(Id* a);

 private:
  Lexer* lex_ {nullptr};   // lexical analyzer for this parser
  Token* look_ {nullptr};  // lookahead token
  Env* top_ {nullptr};     // current or top symbol table
  int used_ {0};           // storage used for declarations
};
