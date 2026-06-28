#include <doctest.h>
#include <memory>
#include <vector>

#include "lexer.h"
#include "nodes.h"
#include "parser.h"

using namespace std;

TEST_CASE("Parser: smoke test") {
  Lexer::instance().setInputString("1+2;");

  vector<unique_ptr<Node>> statements;
  CHECK(stmts(statements));
  CHECK_EQ(statements.size(), 1U);

  Node* n = statements[0].get();
  CHECK(n);
  CHECK_EQ(n->node_type(), ntSum);
  BinOp* bop = dynamic_cast<BinOp*>(n);
  CHECK(bop);
  CHECK(bop->left);
  CHECK(bop->right);
  CHECK_EQ(bop->left->node_type(), ntNum);
  CHECK_EQ(bop->right->node_type(), ntNum);
}

TEST_CASE("Parser: address of test") {
  Lexer::instance().setInputString("int a; #a;");

  vector<unique_ptr<Node>> statements;
  CHECK(stmts(statements));
  REQUIRE_EQ(statements.size(), 2U);  // declaration + address of

  Node* n = statements[1].get();
  CHECK(n);
  CHECK_EQ(n->node_type(), ntAddressOf);
}
