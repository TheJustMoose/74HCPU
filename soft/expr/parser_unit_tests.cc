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
  CHECK_EQ(n->type(), ntSum);
  BinOp* bop = dynamic_cast<BinOp*>(n);
  CHECK(bop);
  CHECK(bop->left);
  CHECK(bop->right);
  CHECK_EQ(bop->left->type(), ntNum);
  CHECK_EQ(bop->right->type(), ntNum);
}
