#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "lexer.h"
#include "token.h"

#include <string>

using namespace std;

TEST_CASE("check Lexer") {
  Lexer l("a");
  CHECK_EQ(l.currentToken(), tName);
}
