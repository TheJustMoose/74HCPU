#pragma once

#include "token.h"

#include <string>

class Lexer {
 public:
  Lexer() {}
  Lexer(string s) {
    setInputString(s);
  }

  void setInputString(string s) {
    input_string_ = s;
    idx_ = 0;
  }

  Token currentToken() {
    return current_token_;
  }

  void consume();

 private:
  void findNextToken();
  char readChar();

  string input_string_ {};
  size_t idx_ {0};

  Token current_token_ {tEnd};
};
