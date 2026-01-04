#pragma once

#include "token.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class Word;

namespace str {
class istream;
}

class Lexer {
 public:
  Lexer(std::istream& is);
  ~Lexer();

  Token* scan();

  // Lexer will own all words and will remove it in dtor
  // So we can return raw pointers
  static Word* get_word(std::string word);

  static int line() {
    return line_;
  }

 protected:
  void reserve(std::shared_ptr<Word> w);
  Token* reserve(Token* t);
  void readch();
  bool readch(char c);

 private:
  char peek_ {' '};
  static int line_;

  std::istream& is_;

  // only one Lexer can work now
  static std::map<std::string, std::shared_ptr<Word>> words_;
  static std::vector<std::shared_ptr<Token>> tokens_;
};
