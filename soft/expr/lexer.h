#pragma once

#include "token.h"

#include <string>

class Lexer {
 public:
  Lexer() {}
  Lexer(std::string s) {
    setInputString(s);
  }

  void setInputString(std::string s) {
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

  std::string input_string_ {};
  size_t idx_ {0};

  int int_value_ {0};
  std::string name_ {};

  Token current_token_ {tEnd};
};
