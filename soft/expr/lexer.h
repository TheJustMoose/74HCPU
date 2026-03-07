#pragma once

#include "token.h"

#include <string>

class Lexer {
 public:
  Lexer() {}
  Lexer(std::string s) {
    setInputString(s);
    consume();  // have to find first token
  }

  void setInputString(std::string s) {
    input_string_ = s;
    idx_ = 0;
  }

  Token currentToken() {
    return current_token_;
  }

  int getIntValue() {
    return int_value_;
  }

  std::string getStrValue() {
    return var_name_;
  }

  void consume();

 private:
  void findNextToken();
  char readChar();

  std::string input_string_ {};
  size_t idx_ {0};

  int int_value_ {0};
  std::string var_name_ {};

  Token current_token_ {tEnd};
};
