#include <cctype>
#include <iostream>
#include <string>

using namespace std;

string input_string;
size_t idx {0};
int value {0};

enum Token {
  tPlus, tNum, tName, tEnd
};

Token GetToken() {
  char c = input_string[idx];
  if (isdigit(c))
    return tNum;
  if (isalpha(c))
    return tName;
  if (c == '+')
    return tPlus;
  return tEnd;
}

int expr() {
  Token t = GetToken();
  return 0;
}

int main(int argc, char* argv[]) {
  cout << "argc: " << argc << endl;
  for (int i = 0; i < argc; i++) {
    cout << "i: " << i << ", arg[i]: " << argv[i] << endl;
  }
  cout << endl;

  if (argc == 2 && argv[1]) {
    cout << "Try to process: " << argv[1] << "..." << endl;
    input_string = argv[1];
  } else {
    cout << "Using: plus.exe \"1+2+3\"" << endl;
    return 1;
  }

  expr();

  return 0;
}
