#include "lexer.h"

#include <cctype>
#include <iostream>

using namespace std;

void Lexer::consume() {
  findNextToken();
}

char Lexer::readChar() {
  return (idx_ < input_string_.size()) ? input_string_[idx_++] : 0;
}

void Lexer::findNextToken() {
  if (idx_ >= input_string_.size()) {
    cout << "tEnd" << endl;
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
    if (idx_ < input_string_.size())  // if we stopped on '\0' char, we should stay there
      idx_--;  // last char was not isdigit so we should step back

    cout << "tNum: " << value << endl;
    int_value_ = value;
    current_token_ = tNum;
    return;
  }

  if (isalpha(c) || c == '_') {
    string name;
    while (isalpha(c) || isdigit(c) || c == '_') {
      name += c;
      c = readChar();  // readChar will return 0 instead of tEnd
    }  // However "if (idx_ >= input_string_.size())" is at the beginning of the method,
       // so no decrement is required here.

    cout << "tName: " << name << endl;
    var_name_ = name;
    current_token_ = tName;
    return;
  }

  switch (c) {
    case '+': /*cout << TokenName[tPlus] << endl;*/ current_token_ = tPlus; return;
    case '-': /*cout << TokenName[tMinus] << endl;*/ current_token_ = tMinus; return;
    case '*': /*cout << TokenName[tMul] << endl;*/ current_token_ = tMul; return;
    case '/': /*cout << TokenName[tDiv] << endl;*/ current_token_ = tDiv; return;
    case '(': /*cout << TokenName[tLBracket] << endl;*/ current_token_ = tLBracket; return;
    case ')': /*cout << TokenName[tRBracket] << endl;*/ current_token_ = tRBracket; return;
    case '=': /*cout << TokenName[tEqual] << endl;*/ current_token_ = tEqual; return;
    case ';': /*cout << TokenName[tSemicolon] << endl;*/ current_token_ = tSemicolon; return;
    default: cout << "Lexer: Unknown token: " << int(c) << endl; current_token_ = tError; break;
  }
}
