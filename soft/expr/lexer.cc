#include "lexer.h"
#include "names.h"

#include <cctype>
#include <iostream>

using namespace std;

// static
Lexer& Lexer::instance() {
  static Lexer lex;
  return lex;
}

void Lexer::consume() {
  findNextToken();
}

char Lexer::readChar() {
  char res = (idx_ < input_string_.size()) ? input_string_[idx_++] : 0;
  return res;
}

void Lexer::findNextToken() {
  if (idx_ >= input_string_.size()) {
    current_token_ = tEnd;
    return;
  }

  char c = readChar();
  if (isspace(c)) {     // if we have space, we have to skip it
    while (isspace(c))  // example: a b c
      c = readChar();
  }

  // Okay, now c != space
  if (isdigit(c)) {
    int value = 0;
    while (isdigit(c)) {
      int dig = c - '0';
      value = value*10 + dig;
      c = readChar();
    }
    if (c)
      idx_--;

    int_value_ = value;
    current_token_ = tNum;
    return;
  }

  if (isalpha(c) || c == '_') {
    string name;
    while (isalpha(c) || isdigit(c) || c == '_') {
      name += c;
      c = readChar();
    }
    if (c)
      idx_--;

    var_name_ = name;
    current_token_ = tName;
    return;
  }

  switch (c) {
    case '+': current_token_ = tPlus; return;
    case '-': current_token_ = tMinus; return;
    case '*': current_token_ = tMul; return;
    case '/': current_token_ = tDiv; return;
    case '(': current_token_ = tLBracket; return;
    case ')': current_token_ = tRBracket; return;
    case '=': current_token_ = tEqual; return;
    case ';': current_token_ = tSemicolon; return;
    default: cout << "Lexer::findNextToken> Unknown token: " << int(c) << endl; current_token_ = tError; break;
  }
}
