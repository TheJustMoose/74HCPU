#include "const.h"

// static
Constant* Constant::true_ {nullptr};
Constant* Constant::false_ {nullptr};

Constant* Constant::True() {
  if (!true_)
    true_ = new Constant(Lexer::get_word("true"), Type::Bool());
  return true_;
}

Constant* Constant::False() {
  if (!false_)
    false_ = new Constant(Lexer::get_word("false"), Type::Bool());
  return false_;
}
