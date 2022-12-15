// Copyright 2022 Daniel Liu

// Function to lay out the contents of a text file on a TUI.

#include "TextFileLayout.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "Utilities.hpp"

namespace dvim {

std::vector<std::string> layoutFile(const std::string& fileContents, unsigned int width) {
  std::vector<std::string> lines;
  std::string line;

  for (char ch : fileContents) {
    if (ch == '\n') {
      lines.emplace_back(line);
      line.clear();
    } else {
      line.push_back(ch);
      if (size(line) >= width) {
        lines.emplace_back(line);
        line.clear();
      }
    }
  }
  if (line != "") {
    lines.emplace_back(line);
  }
  return lines;
}

std::vector<std::string> layoutFileWithLineNums(const std::string& fileContents, unsigned int width) {
  int numLines = 1;
  for (auto c : fileContents) {
    if (c == '\n') ++numLines;
  }
  unsigned int leftPadding = static_cast<unsigned int>(size(std::to_string(numLines))) + 1;

  unsigned int lineNumber = 1;
  std::vector<std::string> lines;
  std::string line = "";
  for (unsigned int i = 0; i < leftPadding - size(std::to_string(lineNumber)); ++i) {
    line.push_back(' ');
  }
  line += "\33[38;5;243m" + std::to_string(lineNumber++) + "\33[0m";
  line += " ";

  for (char ch : fileContents) {
    if (ch == '\n') {
      lines.emplace_back(line);
      line.clear();
      for (unsigned int i = 0; i < leftPadding - size(std::to_string(lineNumber)); ++i) {
        line.push_back(' ');
      }
      line += "\33[38;5;243m" + std::to_string(lineNumber++) + "\33[0m";
      line += " ";
    } else {
      line.push_back(ch);
      if (size(dvim::splitVisibleCharacters(line)) >= width) {
        lines.emplace_back(line);
        line.clear();
        for (unsigned int i = 0; i < leftPadding + 1; ++i) {
          line.push_back(' ');
        }
      }
    }
  }

  if (size(line) > leftPadding + 16 || size(lines) == 0) {
    lines.emplace_back(line);
  }
  return lines;
}

}
