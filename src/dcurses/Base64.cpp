// Copyright 2022 Daniel Liu

// Base 64 implementation

#include "Base64.hpp"

#include <fstream>
#include <stdio.h>
#include <string>

namespace dcurses {

std::string base64Encode(const std::string &input) {
  static const std::string base64Chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

  std::string output;
  auto charAt = [&](unsigned int i) -> unsigned char {
    if (i >= size(input)) {
      return 0;
    }
    return static_cast<unsigned char>(input[i]);
  };
  for (unsigned int i = 0; i < size(input); i += 3) {
    int32_t group = (charAt(i) << 16) | (charAt(i + 1) << 8) | charAt(i + 2);
    output += base64Chars[(group >> 18) & 0x3F];
    output += base64Chars[(group >> 12) & 0x3F];
    if (i + 1 >= size(input)) {
      output += "=";
    } else {
      output += base64Chars[(group >> 6) & 0x3F];
    }
    if (i + 2 >= size(input)) {
      output += "=";
    } else {
      output += base64Chars[group & 0x3F];
    }
  }
  return output;
}

}
