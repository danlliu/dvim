// Copyright 2022 Daniel Liu

// String length utilities.

#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

#include <string>
#include <vector>

namespace dvim {

/*
 * Splits the string into a vector, where each element can be printed out as a one-wide character.
 * Handles splitting color/graphics ANSI escape sequences.
 * @param str The string to split.
 * @return A vector of strings, where each string is a one-wide character
 */
std::vector<std::string> splitVisibleCharacters(const std::string &str);
  
}  // namespace dvim

#endif
