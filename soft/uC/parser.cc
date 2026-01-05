#include "parser.h"

#include <stdexcept>

#include "access.h"
#include "and.h"
#include "arith.h"
#include "array.h"
#include "const.h"
#include "env.h"
#include "expr.h"
#include "id.h"
#include "lexer.h"
#include "logical.h"
#include "or.h"
#include "not.h"
#include "rel.h"
#include "seq.h"
#include "stmt.h"
#include "tag.h"
#include "token.h"
#include "unary.h"
#include "word.h"

using namespace std;

Parser::Parser(Lexer* l)
  : lex_(l) {
  cout << __func__ << endl;
  pmove();
}

void Parser::pmove() {
  cout << __func__ << endl;
  look_ = lex_->scan();
  if (look_)
    cout << "look_: " << look_->toString() << endl;
  else
    error("look_ == nullptr is not acceptable");
}

void Parser::error(string s) {
  throw runtime_error("near line " + to_string(lex_->line()) + ": " + s);
}

void Parser::match(Tag t)  {
  cout << __func__ << endl;
  if (look_->tag() == t)
    pmove();
  else
    error("syntax error, waiting for: " + tag::toString(t));
}

void Parser::match(char c) {
  cout << __func__ << endl;
  if (tag::cTag(look_->tag()) == c)
    pmove();
  else {
    string s;
    s += c;
    error("syntax error, waiting for: " + s);
  }
}

void Parser::program() {
  cout << __func__ << endl;
  Stmt* s = block();
  if (!s)
    return;

  int begin = s->newlabel();
  int after = s->newlabel();
  s->emitlabel(begin);
  s->gen(begin, after);
  s->emitlabel(after);
}

Stmt* Parser::block() {  // block -> { decls stmts }
  cout << __func__ << endl;
  match('{');
  Env* savedEnv = top_;
  top_ = new Env(top_);
  decls();
  Stmt* s = stmts();
  match('}');
  delete top_;
  top_ = savedEnv;
  return s;
}

void Parser::decls() {
  cout << __func__ << endl;
  while (look_ && (look_->tag() == Tag::tBASIC)) {   // D -> type ID ;
    Type* p = type();
    if (!p)
      error("p is null");
    Token* tok = look_;
    match(Tag::tID);
    match(';');

    Id* id = new Id(static_cast<Word*>(tok), p, used_);  // check it: who own this var?
    top_->put(tok, id);  // check it: who own this var?
    used_ += p->width();
  }
}

Type* Parser::type() {
  cout << __func__ << endl;
  Type* p = IsType(look_) ? static_cast<Type*>(look_) : nullptr;  // expect look_->tag == Tag::tBASIC
  match(Tag::tBASIC);
  if (tag::cTag(look_->tag()) != '[')
    return p;            // T -> basic
  else
    return dims(p);   // return array type
}

Type* Parser::dims(Type* p) {
  match('[');
  Token* tok = look_;
  match(Tag::tNUM);
  match(']');
  if (tag::cTag(look_->tag()) == '[')
    p = dims(p);
  return new Array(((Num*)tok)->value(), p);
}

Stmt* Parser::stmts() {
  cout << __func__ << endl;
  if (tag::cTag(look_->tag()) == '}')
    return Stmt::Null();
  else {
    Stmt* tmp = stmt();  // C++ optimizer can call stmts() before stmt()
    return new Seq(tmp, stmts());  // so I try to repair call order
  }
}

Stmt* Parser::stmt() {
  cout << __func__ << endl;
  Expr* x {nullptr};
  Stmt* s {nullptr};
  Stmt* s1 {nullptr};
  Stmt* s2 {nullptr};
  Stmt* savedStmt {nullptr};  // save enclosing loop for breaks

  switch (look_->tag()) {
    case Tag::tSEMI: {
      pmove();
      return Stmt::Null();
    }

    case Tag::tIF: {
      match(Tag::tIF); match('('); x = boolean(); match(')');
      s1 = stmt();
      if (look_->tag() != Tag::tELSE)
        return new If(x, s1);
      match(Tag::tELSE);
      s2 = stmt();
      return new Else(x, s1, s2);
    }

    case Tag::tWHILE: {
      While* whilenode = new While();
      savedStmt = Stmt::Enclosing;
      Stmt::Enclosing = whilenode;
      match(Tag::tWHILE); match('('); x = boolean(); match(')');
      s1 = stmt();
      whilenode->init(x, s1);
      Stmt::Enclosing = savedStmt;  // reset Stmt.Enclosing
      return whilenode;
    }

    case Tag::tDO: {
      Do* donode = new Do();
      savedStmt = Stmt::Enclosing;
      Stmt::Enclosing = donode;
      match(Tag::tDO);
      s1 = stmt();
      match(Tag::tWHILE); match('('); x = boolean(); match(')'); match(';');
      donode->init(s1, x);
      Stmt::Enclosing = savedStmt;  // reset Stmt.Enclosing
      return donode;
    }

    case Tag::tBREAK: {
      match(Tag::tBREAK); match(';');
      return new Break();
    }

    case Tag::tLBRACE: {  // '{'
      return block();
    }

    default: {
      return assign();
    }
  }
}

Stmt* Parser::assign() {
  cout << __func__ << endl;
  Stmt* stmt {nullptr};
  Token* t {look_};
  match(Tag::tID);
  Id* id = top_->get(t);
  if (id == nullptr)
    error(t->toString() + " undeclared");

  if (tag::cTag(look_->tag()) == '=') { // S -> id = E ;
    pmove();
    stmt = new Set(id, boolean());
  } else {                        // S -> L = E ;
    Access* x = offset(id);
    match('=');
    stmt = new SetElem(x, boolean());
  }
  match(';');
  return stmt;
}

Expr* Parser::boolean() {
  Expr* x = join();
  while (look_->tag() == Tag::tOR) {
    Token* tok {look_};
    pmove();
    x = new Or(tok, x, join());
  }

  return x;
}

Expr* Parser::join() {
  Expr* x = equality();
  while (look_->tag() == Tag::tAND) {
    Token* tok {look_};
    pmove();
    x = new And(tok, x, equality());
  }

  return x;
}

Expr* Parser::equality() {
  Expr* x = rel();
  while (look_->tag() == Tag::tEQ || look_->tag() == Tag::tNE) {
    Token* tok {look_};
    pmove();
    x = new Rel(tok, x, rel());
  }

  return x;
}

Expr* Parser::rel() {
  Expr* x = expr();
  switch (look_->tag()) {
    case Tag::tLess:
    case Tag::tLE:
    case Tag::tGE:
    case Tag::tMore: {
      Token* tok {look_};
      pmove();
      return new Rel(tok, x, expr());
    }
    default:
      return x;
  }
}

Expr* Parser::expr() {
  Expr* x = term();
  while (tag::cTag(look_->tag()) == '+' ||
         tag::cTag(look_->tag()) == '-') {
    Token* tok {look_};
    pmove();
    x = new Arith(tok, x, term());
  }

  return x;
}

Expr* Parser::term() {
  Expr* x = unary();
  while (tag::cTag(look_->tag()) == '*' ||
         tag::cTag(look_->tag()) == '/') {
    Token* tok {look_};
    pmove();
    x = new Arith(tok, x, unary());
  }

  return x;
}

Expr* Parser::unary() {
  if (tag::cTag(look_->tag()) == '-') {
    pmove();
    return new Unary(Lexer::get_word("minus"), unary());
  } else if (tag::cTag(look_->tag()) == '!') {
    Token* tok {look_};
    pmove();
    return new Not(tok, unary());
  }
  else
    return factor();
}

Expr* Parser::factor() {
  Expr* x {nullptr};
  switch (look_->tag()) {
    case Tag::tPARENTHESES:  // '('
      pmove();
      x = boolean();
      match(')');
      return x;
    case Tag::tNUM:
      x = new Constant(look_, Type::Int());
      pmove();
      return x;
    case Tag::tREAL:
      x = new Constant(look_, Type::Float());
      pmove();
      return x;
    case Tag::tTRUE:
      x = Constant::True();
      pmove();
      return x;
    case Tag::tFALSE:
      x = Constant::False();
      pmove();
      return x;
    case Tag::tID: {
      //string s = look_->toString();  ??????
      Id* id = top_->get(look_);
      if (id == nullptr)
        error(look_->toString() + " undeclared");
      pmove();
      if (tag::cTag(look_->tag()) != '[')
        return id;
      else
        return offset(id);
    }
    default:
      error("syntax error");
      return x;
  }
}

Access* Parser::offset(Id* a) {   // I -> [E] | [E] I
  Expr* i {nullptr};
  Expr* w {nullptr};
  Expr* t1 {nullptr};
  Expr* t2 {nullptr};
  Expr* loc {nullptr};  // inherit id

  Type* type = a->type();
  match('['); i = boolean(); match(']');     // first index, I -> [ E ]
  type = ((Array*)type)->of_;
  w = new Constant(type->width());
  t1 = new Arith(new Token('*'), i, w);
  loc = t1;
  while (tag::cTag(look_->tag()) == '[') {      // multi-dimensional I -> [ E ] I
    match('['); i = boolean(); match(']');
    type = ((Array*)type)->of_;
    w = new Constant(type->width());
    t1 = new Arith(new Token('*'), i, w);
    t2 = new Arith(new Token('+'), loc, t1);
    loc = t2;
  }

  return new Access(a, loc, type);
}
