#include "lexer.h"
#include "names.h"

#include <cctype>
#include <iostream>

using namespace std;

void Lexer::consume() {
  cout << "Lexer::consume()" << endl;
  findNextToken();
}

char Lexer::readChar() {
  cout << "begin of readChar(): idx_: " << idx_ << endl;
  char res = (idx_ < input_string_.size()) ? input_string_[idx_++] : 0;
  //cout << "end of readChar(): idx_: " << idx_ << endl;
  return res;
}

void Lexer::findNextToken() {
  cout << "Lexer::findNextToken> idx_: " << idx_ << endl;
  if (idx_ >= input_string_.size()) {
    cout << "Lexer::findNextToken> tEnd cause idx_ > size()" << endl;
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
      c = readChar();  // есть ощущение, что этого символа мы больше не увидим... :(
    }
    if (c)
      idx_--;  // туда: проверить, а что если цифра стоит в конце строки?

    cout << "Lexer::findNextToken> tNum: " << value << endl;
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

    cout << "Lexer::findNextToken> tName: " << name << endl;
    var_name_ = name;
    current_token_ = tName;
    return;
  }

  switch (c) {
    case '+': cout << GetTokenName(tPlus) << endl; current_token_ = tPlus; return;
    case '-': cout << GetTokenName(tMinus) << endl; current_token_ = tMinus; return;
    case '*': cout << GetTokenName(tMul) << endl; current_token_ = tMul; return;
    case '/': cout << GetTokenName(tDiv) << endl; current_token_ = tDiv; return;
    case '(': cout << GetTokenName(tLBracket) << endl; current_token_ = tLBracket; return;
    case ')': cout << GetTokenName(tRBracket) << endl; current_token_ = tRBracket; return;
    case '=': cout << GetTokenName(tEqual) << endl; current_token_ = tEqual; return;
    case ';': cout << GetTokenName(tSemicolon) << endl; current_token_ = tSemicolon; return;
    default: cout << "Lexer::findNextToken> Unknown token: " << int(c) << endl; current_token_ = tError; break;
  }
}
