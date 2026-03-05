#include "lexer.h"

void Lexer::consume() {
  findNextToken();
}

char Lexer::readChar() {
  return (idx_ < input_string_.size()) ? input_string_[idx_++] : 0;
}

void Lexer::findNextToken() {
  if (idx >= input_string.size()) {
    cout << "tEnd" << endl;
    current_token_ = tEnd;
    return;
  }

  char c = readChar();
  if (isdigit(c)) {
    value = 0;
    while (isdigit(c)) {
      int dig = c - '0';
      value = value*10 + dig;
      c = readChar();
    }
    idx--;  // last char was not isdigit so we should step back
    cout << "tNum" << endl;
    current_token_ = tNum;
    return;
  }

  if (isalpha(c) || c == '_') {
    string name;
    name += c;
    if (isalpha(c) || isdigit(c) || c == '_') {
      name += c;
      c = readChar();
    }

    cout << "tName: " << name << endl;
    current_token_ = tName;
    return;
  }

  switch (c) {
    case '+': cout << TokenName[tPlus] << endl; current_token_ = tPlus; break;
    case '-': cout << TokenName[tMinus] << endl; current_token_ = tMinus; break;
    case '*': cout << TokenName[tMul] << endl; current_token_ = tMul; break;
    case '/': cout << TokenName[tDiv] << endl; current_token_ = tDiv; break;
    case '(': cout << TokenName[tLBracket] << endl; current_token_ = tLBracket; break;
    case ')': cout << TokenName[tRBracket] << endl; current_token_ = tRBracket; break;
    case '=': cout << TokenName[tEqual] << endl; current_token_ = tEqual; break;
    case ';': cout << TokenName[tSemicolon] << endl; current_token_ = tSemicolon; break;
  }

  cout << "Lexer: Unknown token: " << c << endl;
  current_token_ = tError;
  return;
}
