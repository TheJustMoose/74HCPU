#pragma once

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

  char currentToken() {
    return (idx_ < input_string_.size()) ?
        input_string_[idx_] : 0;
  }

  void consume() {
    if (idx_ < input_string_.size())
      idx_++;
  }

 private:
  string input_string_ {};
  size_t idx_ {0};
};
