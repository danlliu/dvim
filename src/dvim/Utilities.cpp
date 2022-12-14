// Copyright 2022 Daniel Liu

// String length utilities.

#include "Utilities.hpp"

#include <regex>
#include <string>
#include <vector>

namespace dvim {

std::string escapeString(const std::string &str) {
  auto result = str;
  result = std::regex_replace(result, std::regex("\\n"), "\\n");
  result = std::regex_replace(result, std::regex("\33"), "\\33");
  std::string escaped = "";
  for (auto c : result) {
    if (c < 32 || c > 126) {
      escaped += "\\x" + std::to_string(c);
    } else {
      escaped += c;
    }
  }
  return escaped;
}

std::vector<std::string> splitVisibleCharacters(const std::string &str) {
  std::vector<std::string> result;
  std::string color = "";
  std::string currentFragment = "";
  std::string fragment = "";
  bool escaping = false;
  int unicoding = 0;
  for (auto c : str) {
    if (c == '\33') {
      escaping = true;
    }
    if ((c & 0xe0) == 0xc0) unicoding = 1;
    if ((c & 0xf0) == 0xe0) unicoding = 2;
    if ((c & 0xf8) == 0xf0) unicoding = 3;
    if ((c & 0xc0) == 0x80) unicoding--;
    currentFragment += c;
    fragment += c;
    if (!escaping && !unicoding) {
      if (color != "") fragment += "\33[0m";
      result.emplace_back(fragment);
      fragment = color;
      currentFragment = "";
    }
    if (escaping) {
      if (c == 'm') {
        escaping = false;
        if (currentFragment == "\33[0m") {
          color = "";
          fragment = "";
          currentFragment = "";
        } else {
          color += fragment;
        }
      }
    }
  }
  return result;
}

}
