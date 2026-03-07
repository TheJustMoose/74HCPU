#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "lexer.h"
#include "token.h"

#include <cctype>
#include <string>

using namespace std;

struct TestToken {
  Token token {tEnd};
  string text {""};
  int number {0};

  TestToken(Token t, string s, int i)
    : token(t), text(s), number(i) {}
};

void CheckLexerOutput(Lexer& l, const vector<TestToken>& right) {
  vector<TestToken> left;
  Token t {tEnd};
  do {
    t = l.currentToken();
    string s = l.getStrValue();
    int i = l.getIntValue();
    left.emplace_back(t, s, i);
    l.consume();
  } while (t != tEnd && t != tError);

  CHECK_EQ(left.size(), right.size());
  for (size_t i = 0; i < left.size(); i++) {
    CHECK_EQ(left[i].token, right[i].token);
    if (right[i].token == tName)
      CHECK_EQ(left[i].text, right[i].text);
    if (right[i].token == tNum)
      CHECK_EQ(left[i].number, right[i].number);
  }
}

TEST_CASE("check C++") {
  CHECK(isalpha('a'));
  CHECK(isalpha('Z'));

  CHECK( ! isalpha(' '));  // space is not isalpha
  CHECK_EQ(int(' '), 32);

  CHECK(isspace(' '));
  CHECK(isspace('\t'));
}

TEST_CASE("check Lexer") {
  Lexer l("a");
  CHECK_EQ(l.currentToken(), tName);
  CHECK_EQ(l.getStrValue(), "a");

  Lexer l2("abcd");
  CHECK_EQ(l2.currentToken(), tName);
  CHECK_EQ(l2.getStrValue(), "abcd");

  Lexer l3("123");
  CHECK_EQ(l3.currentToken(), tNum);
  CHECK_EQ(l3.getIntValue(), 123);

  Lexer l4("1");
  CHECK_EQ(l4.currentToken(), tNum);
  CHECK_EQ(l4.getIntValue(), 1);

  Lexer l5("65535");
  CHECK_EQ(l5.currentToken(), tNum);
  CHECK_EQ(l5.getIntValue(), 65535);
}

TEST_CASE("check Lexer tokens") {
  Lexer l("+");
  CHECK_EQ(l.currentToken(), tPlus);

  Lexer l2("-");
  CHECK_EQ(l2.currentToken(), tMinus);

  Lexer l3("*");
  CHECK_EQ(l3.currentToken(), tMul);

  Lexer l4("/");
  CHECK_EQ(l4.currentToken(), tDiv);

  Lexer l5("(");
  CHECK_EQ(l5.currentToken(), tLBracket);

  Lexer l6(")");
  CHECK_EQ(l6.currentToken(), tRBracket);

  Lexer l7("=");
  CHECK_EQ(l7.currentToken(), tEqual);

  Lexer l8(";");
  CHECK_EQ(l8.currentToken(), tSemicolon);

  Lexer l9("#");
  CHECK_EQ(l9.currentToken(), tError);
}

TEST_CASE("check Lexer and many long strings") {
  Lexer l("abcd efgh");

  vector<TestToken> r {
    {tName, "abcd", 0},
    {tName, "efgh", 0},
    {tEnd, "", 0},
  };

  CheckLexerOutput(l, r);
}

TEST_CASE("check Lexer and many numbers") {
  Lexer l("12 234 56789");

  vector<TestToken> r {
    {tNum, "", 12},
    {tNum, "", 234},
    {tNum, "", 56789},
    {tEnd, "", 0},
  };

  CheckLexerOutput(l, r);
}

TEST_CASE("check Lexer and numbers and string") {
  Lexer l("12 234 abc");
  CHECK_EQ(l.currentToken(), tNum);
  CHECK_EQ(l.getIntValue(), 12);

  l.consume();

  CHECK_EQ(l.currentToken(), tNum);
  CHECK_EQ(l.getIntValue(), 234);

  l.consume();

  CHECK_EQ(l.currentToken(), tName);
  CHECK_EQ(l.getStrValue(), "abc");

  l.consume();

  CHECK_EQ(l.currentToken(), tEnd);
}

TEST_CASE("check Lexer: Okay, now we can check arithm") {
  Lexer l("12+234-56");
  CHECK_EQ(l.currentToken(), tNum);
  CHECK_EQ(l.getIntValue(), 12);

  l.consume();

  CHECK_EQ(l.currentToken(), tPlus);

  l.consume();

  CHECK_EQ(l.currentToken(), tNum);
  CHECK_EQ(l.getIntValue(), 234);

  l.consume();

  CHECK_EQ(l.currentToken(), tMinus);

  l.consume();

  CHECK_EQ(l.currentToken(), tNum);
  CHECK_EQ(l.getIntValue(), 56);

  l.consume();

  CHECK_EQ(l.currentToken(), tEnd);

  Lexer l2(" 12 * 234 ");
  CHECK_EQ(l2.currentToken(), tNum);
  CHECK_EQ(l2.getIntValue(), 12);

  l2.consume();

  CHECK_EQ(l2.currentToken(), tMul);

  l2.consume();

  CHECK_EQ(l2.currentToken(), tNum);
  CHECK_EQ(l2.getIntValue(), 234);

  l2.consume();

  CHECK_EQ(l2.currentToken(), tEnd);
}
