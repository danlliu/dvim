// Copyright 2022 Daniel Liu

// Object for representing an editor window.

#include "Editor.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace dvim {

Editor::Editor(const std::filesystem::path &path) {
  std::string line;
  std::ifstream fin(path);
  while (std::getline(fin, line)) {
    lines_.emplace_back(std::list<char>(begin(line), end(line)));
  }
  cursorLineIterator_ = begin(lines_);
  cursorColIterator_ = begin(*cursorLineIterator_);
}

void Editor::handleInput(char ch) {

}

std::vector<std::string> Editor::getLines(unsigned int width) {

  unsigned int textWidth = width - static_cast<unsigned int>(size(std::to_string(size(lines_)))) - 2;
  unsigned int paddingWidth = width - textWidth;

  unsigned int lineNumber = 1;
  std::vector<std::string> lines;
  for (auto lit = begin(lines_); lit != end(lines_); ++lit) {
    std::string line = "";
    line += "\33[38;5;243m" + std::to_string(lineNumber) + "\33[0m";
    line += " ";
    line = std::string(paddingWidth - static_cast<unsigned int>(size(std::to_string(lineNumber))) - 1, ' ') + line;
    ++lineNumber;

    for (auto cit = begin(*lit); cit != end(*lit); ++cit) {
      if (lit == cursorLineIterator_ && cit == cursorColIterator_) {
        line += "\33[48;5;243m" + std::string{*cit} + "\33[0m";
        cursorScroll_ = static_cast<unsigned int>(size(lines));
      } else {
        line += *cit;
      }
      if (size(line) >= textWidth) {
        lines.emplace_back(line);
        line = " ";
        for (unsigned int i = 0; i < paddingWidth; ++i) line += ' ';
      }
    }
    if (cursorColIterator_ == end(*lit) && lit == cursorLineIterator_) {
      line += "\33[48;5;243m \33[0m";
    }
    lines.emplace_back(line);
  }
  if (cursorLineIterator_ == end(lines_)) {
    std::string line = "\33[48;5;243m \33[0m";
    lines.emplace_back(line);
  }
  return lines;
}

}
