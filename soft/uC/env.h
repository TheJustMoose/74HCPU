#pragma once

#include <map>
#include <memory>

class Id;
class Token;

class Env {
 public:
  typedef std::map<Token*, Id*> SymTable;

  Env(Env* n = nullptr) {
    prev_ = n;
  }

  void put(Token* w, Id* id) {
    table_[w] = id;
  }

  Id* get(Token* w) {
    for (Env* e = this; e != nullptr; e = e->prev_) {
      SymTable::iterator it = e->table_.find(w);
      if (it != e->table_.end())
        return it->second;
    }
    return nullptr;
  }

 protected:
  Env* prev_ {nullptr};

 private:
  SymTable table_;  // see above
};
