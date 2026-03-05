#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "lexer.h"
#include "token.h"

#include <string>

using namespace std;

TEST_CASE("check Lexer") {
  Lexer l("a");
  CHECK_EQ(l.currentToken(), tName);
  CHECK_EQ(l.getVarName(), "a");

  Lexer l2("abcd");
  CHECK_EQ(l2.currentToken(), tName);
  CHECK_EQ(l2.getVarName(), "abcd");

  Lexer l3("123");
  CHECK_EQ(l3.currentToken(), tNum);
  CHECK_EQ(l3.getIntValue(), 123);
}
