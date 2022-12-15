// Copyright 2022 Daniel Liu

// Object for representing an editor window.

#include "Editor.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Utilities.hpp"

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
  switch (mode) {
    case EditorMode::NORMAL:
      switch (ch) {
        case 'h':
          if (cursorColumn_ != 0) {
            --cursorColIterator_;
            --cursorColumn_;
          }
          break;

        case 'j':
          if (cursorLine_ != size(lines_) - 1) {
            ++cursorLineIterator_;
            ++cursorLine_;
            if (cursorColumn_ >= size(*cursorLineIterator_)) {
              if (size(*cursorLineIterator_) == 0) {
                cursorColumn_ = 0;
              } else {
                cursorColumn_ = static_cast<unsigned int>(size(*cursorLineIterator_) - 1);
              }
            }
            cursorColIterator_ = begin(*cursorLineIterator_);
            std::advance(cursorColIterator_, cursorColumn_);
          }
          break;

        case 'k':
          if (cursorLine_ != 0) { 
            --cursorLineIterator_;
            --cursorLine_;
            if (cursorColumn_ >= size(*cursorLineIterator_)) {
              if (size(*cursorLineIterator_) == 0) {
                cursorColumn_ = 0;
              } else {
                cursorColumn_ = static_cast<unsigned int>(size(*cursorLineIterator_) - 1);
              }
            }
            cursorColIterator_ = begin(*cursorLineIterator_);
            std::advance(cursorColIterator_, cursorColumn_);
          }
          break;

        case 'l':
          if (cursorColumn_ != size(*cursorLineIterator_) - 1) {
            ++cursorColIterator_;
            ++cursorColumn_;
          }
          break;

        case 'x':
          // Delete character at cursor.
          {
            if (cursorColIterator_ == end(*cursorLineIterator_)) {
              break;
            }
            auto newColIterator = cursorColIterator_;
            ++newColIterator;
            if (newColIterator == end(*cursorLineIterator_)) {
              newColIterator = cursorColIterator_;
              --newColIterator;
            }
            cursorLineIterator_->erase(cursorColIterator_);
            --cursorColumn_;
            cursorColIterator_ = newColIterator;
          }
          break;
      }
      break;
    case EditorMode::INSERT:
      break;
    case EditorMode::COMMAND:
      break;
    case EditorMode::VISUAL:
      break;
  }
}

std::vector<std::string> Editor::getUsageHints() const {
  std::vector<std::string> hints;
  switch (mode) {
    case EditorMode::NORMAL:
      return {
        "h - move left",
        "j - move down",
        "k - move up",
        "l - move right",
        "i - insert character",
        "a - append character",
        "x - delete character"
      };
    case EditorMode::INSERT:
      return {
        "ESC - exit insert mode"
      };
    case EditorMode::COMMAND:
      return {
        "ESC - exit command mode"
      };
    case EditorMode::VISUAL:
      return {
        "ESC - exit visual mode"
      };
  }
  return hints;
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
      if (size(dvim::splitVisibleCharacters(line)) >= textWidth) {
        lines.emplace_back(line);
        line = std::string(paddingWidth, ' ');
      }
    }
    if (cursorColIterator_ == end(*lit) && lit == cursorLineIterator_) {
      line += "\33[48;5;243m \33[0m";
      cursorScroll_ = static_cast<unsigned int>(size(lines));
    }
    lines.emplace_back(line);
  }
  if (cursorLineIterator_ == end(lines_)) {
    std::string line = "\33[48;5;243m \33[0m";
    cursorScroll_ = static_cast<unsigned int>(size(lines));
    lines.emplace_back(line);
  }
  return lines;
}

}
