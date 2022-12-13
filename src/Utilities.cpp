// Copyright 2022 Daniel Liu

// String length utilities.

#include "Utilities.hpp"

#include <string>
#include <vector>

namespace dvim {

std::vector<std::string> splitVisibleCharacters(const std::string &str) {
  std::vector<std::string> result;
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
    fragment += c;
    if (!escaping && !unicoding) {
      result.emplace_back(fragment);
      fragment = "";
    }
    if (escaping) {
      if (c == 'm') {
        escaping = false;
      }
    }
  }
  return result;
}

}
