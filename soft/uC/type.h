#pragma once

#include "word.h"

#include <string>

class Token;

class Type: public Word {
 public:
  Type(std::string s, Tag tag, int w)
    : Word(s, tag), width_(w) {}

  int width() { return width_; }

  static bool numeric(Type* p);
  static Type* max(Type* p1, Type* p2);

  static Type* Bool();
  static Type* Char();
  static Type* Int();
  static Type* Float();

  virtual bool is_array() { return false; }

  virtual std::string toString() { return ""; }

 private:
  int width_ {0};
};

bool IsType(Token* t);
