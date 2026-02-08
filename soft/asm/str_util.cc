#include <algorithm>
#include <stdexcept>
#include <string>

#include "str_util.h"

using namespace std;

string ToUpper(string s) {
  std::transform(s.begin(), s.end(), s.begin(), ::toupper);
  return s;
}

// will remove leading and trailing spaces
// also remove more than one space in a row
// if this spaces are not in string literal
string NormalizeLine(string s) {
  string res;
  bool skip_space {true};
  bool in_string {false};
  for (size_t i = 0; i < s.size(); i++) {
    if (s[i] == '"')
      in_string = !in_string;

    if (!in_string) {
      if (IsSpace(s[i])) {
        if (skip_space)
          continue;
        else
          skip_space = true;
      } else {
        skip_space = false;
      }

      if (s[i] == ';')  // also remove comments
        break;
    }
    res += s[i];
  }

  while (res.size() && IsSpace(*res.rbegin()))
    res.resize(res.size() - 1);

  return res;
}

string RemoveQuotes(string s) {
  if (s.size() <= 2)
    return s;

  if (*s.begin() != '\"' || *s.rbegin() != '\"')
    return s;

  return s.substr(1, s.size() - 2);
}

bool StrToInt(string val, int* pout, string* err) {
  int res = 0;

  // Also check chars (for example 'a')
  if (val.size() >= 2 && *val.begin() == '\'' && *val.rbegin() == '\'') {
    if (val.size() != 3) {
      if (err)
        *err = "Can parse only one character 'string' :)";
      return false;
    }
    if (pout)
      *pout = val[1];  // return ASCII code of char
    return true;
  }

  try {
    if (val.find("0X") == 0)
      res = stoi(&val[2], nullptr, 16);
    else if (*val.rbegin() == 'H')
      res = stoi(&val[0], nullptr, 16);
    else if (val.find("0O") == 0)
      res = stoi(&val[2], nullptr, 8);
    else if (val.find("0B") == 0)
      res = stoi(&val[2], nullptr, 2);
    else {
      res = stoi(val, nullptr, 10);
    }
  } catch(std::invalid_argument& e) {
    if (err) {
      *err += e.what();
      *err += "\n";
      *err += "arg: ";
      *err += val;
    }
    return false;
  }

  if (pout)
    *pout = res;
  return true;
}

bool IsSpace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool IsDelimeter(char ch) {
  return ch == ',' || ch == '(' || ch == ')' || ch == '+' || ch == '-';
}

vector<string> Split(string line) {
  vector<string> res;

  string str;
  for (char ch : line) {
    bool delim = IsDelimeter(ch);
    if (IsSpace(ch) || delim) {
      if (str.size()) {
        res.push_back(str);
        str = "";
      }
      if (delim) {
        string dstr {ch};
        res.push_back(dstr);
      }
    } else {
      str += ch;
    }
  }

  if (str.size())
    res.push_back(str);

  return res;
}

vector<string> SplitToCmdParts(string text) {
  vector<string> parts = Split(ToUpper(text));
  if (parts.size() == 0)
    return { "", "", "" };

  // first part is operation name
  // and it has only one word
  string op_name;
  if (parts.size() >= 1)
    op_name = parts[0];

  size_t i = 1;

  string left;
  for (; i < parts.size() && parts[i] != ","; i++) {
    left += parts[i];  // R0 or XI or XI+5
  }

  if (i < parts.size() && parts[i] == ",")
    i++;

  string right;
  for (; i < parts.size(); i++)
    right += parts[i];  // R0 or XI or XI+5

  return { op_name, left, right };
}

string TrimRight(string s) {
  if (s.empty())
    return {};

  int cnt {0};
  string::reverse_iterator it;
  for (it = s.rbegin(); it != s.rend(); it++)
    if (*it == ' ')
      cnt++;
    else
      break;

  if (cnt > 0)
    s.resize(s.size() - cnt);

  return s;
}

string TrimLeft(string s) {
  if (s.empty())
    return {};

  int cnt {0};
  string::iterator it;
  for (it = s.begin(); it != s.end(); it++)
    if (*it == ' ')
      cnt++;
    else
      break;

  if (cnt > 0)
    s = s.erase(0, cnt);

  return s;
}

string Trim(string s) {
  return TrimLeft(TrimRight(s));
}

string Join(vector<string> v, char del) {
  string res {};
  for (size_t i = 0; i < v.size(); i++) {
    res += v[i];
    if (i < v.size() - 1)
      res += del;
  }
  return res;
}

string JoinByte(vector<uint8_t> v, char del) {
  string res {};
  for (size_t i = 0; i < v.size(); i++) {
    res += to_string(v[i]);
    if (i < v.size() - 1)
      res += del;
  }
  return res;
}

string JoinInt(vector<uint16_t> v, char del) {
  string res {};
  for (size_t i = 0; i < v.size(); i++) {
    res += to_string(v[i]);
    if (i < v.size() - 1)
      res += del;
  }
  return res;
}
