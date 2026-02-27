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

// это прототип, чтобы поиграть в деревья
// поэтому пока считаем, что числа и цифры - это одно и то же,
// из операций есть только плюс, и в строке нет пробелов,
// только цифры и плюсы!
Token GetToken() {
  if (idx >= input_string.size())
    return tEnd;

  char c = input_string[idx++];
  if (isdigit(c)) {
    value = c - '0';
    return tNum;
  }

  if (isalpha(c))
    return tName;

  if (c == '+')
    return tPlus;

  return tEnd;
}

int expr() {
  Token t = GetToken();
  if (t == tNum)
    return value;
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

  cout << "res: " << expr() << endl;

  return 0;
}
