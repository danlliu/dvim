// Copyright 2022 Daniel Liu

// Base 64 implementation.

#ifndef DCURSES_BASE64_HPP_
#define DCURSES_BASE64_HPP_

#include <fstream>
#include <string>

namespace dcurses {

std::string base64Encode(std::filebuf &input);
std::string base64Encode(const std::string &input);

}  // namespace dcurses

#endif
