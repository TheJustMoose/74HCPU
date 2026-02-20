#pragma once

#include <iostream>
#include <map>
#include <string>

class ErrorCollector {
 public:
  static ErrorCollector& GetInstance();

  void err(std::string msg, int line) {
    errors_[line] = msg;
  }
  void rep(std::string msg, int line) {
    msgs_[line] = msg;
  }

  void clr(int line) {
    auto eit = errors_.find(line);
    if (eit != errors_.end())
      errors_.erase(eit);

    auto mit = msgs_.find(line);
    if (msgs_.find(line) != msgs_.end())
      msgs_.erase(mit);
  }

  std::map<int, std::string> get() {
    return errors_;
  }

  std::string get(int line, bool& err) {
    err = false;
    if (errors_.find(line) != errors_.end()) {
      err = true;
      return errors_[line];
    } else if (msgs_.find(line) != msgs_.end()) {
      return msgs_[line];
    } else {
      return "";
    }
  }

  bool have_errors() {
    return errors_.size() > 0;
  }

 private:
  std::map<int, std::string> errors_;
  std::map<int, std::string> msgs_;
};
