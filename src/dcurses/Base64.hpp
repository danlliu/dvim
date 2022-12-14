// Copyright 2022 Daniel Liu

// Base 64 implementation.

#ifndef DCURSES_BASE64_HPP_
#define DCURSES_BASE64_HPP_

#include <string>

namespace dcurses {

/*
 * Encodes the provided string into base64 representation.
 * @param input The string to encode.
 * @return The base64 representation of the input.
 */
std::string base64Encode(const std::string &input);

}  // namespace dcurses

#endif
