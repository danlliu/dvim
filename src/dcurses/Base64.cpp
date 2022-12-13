// Copyright 2022 Daniel Liu

// Base 64 implementation

#include "Base64.hpp"

#include <fstream>
#include <stdio.h>
#include <string>

namespace dcurses {

std::string base64Encode(std::filebuf &input) {
  static const std::string base64Chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
  char group[3] = {0, 0, 0};
  std::string output;
  long count = 0;
  while ((count = input.sgetn(group, 3))) {
    // i'm sorry sgetn insisted on using signed chars :(
    int32_t value = ((unsigned char) group[0] << 16) | ((unsigned char) group[1] << 8) | (unsigned char) group[2];
    output += base64Chars[(value >> 18) & 0x3F];
    output += base64Chars[(value >> 12) & 0x3F];
    if (count == 1) {
      output += "=";
    } else {
      output += base64Chars[(value >> 6) & 0x3F];
    }
    if (count == 2) {
      output += "=";
    } else {
      output += base64Chars[value & 0x3F];
    }
  }
  return output;
}

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
    return (unsigned char) input[i];
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
