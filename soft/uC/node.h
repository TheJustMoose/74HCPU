#pragma once

#include "lexer.h"

#include <iostream>
#include <string>
#include <stdexcept>

class Node {
 public:
  Node() { lexline_ = Lexer::line(); }

  int newlabel() { return ++labels_; }
  void emitlabel(int i) { std::cout << "L" << i << ":"; }
  void emit(std::string s) { std::cout << "\t" << s << std::endl; }

  void error(std::string s) {
    throw new std::runtime_error("near line " + std::to_string(lexline_) + ": " + s);
  }

 private:
  int lexline_ {0};
  static int labels_;
};
