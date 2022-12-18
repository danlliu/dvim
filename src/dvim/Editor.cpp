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

Editor::Editor(const std::filesystem::path &path) : path_(path) {
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
    case EditorMode::STOPPED:
      return;
    case EditorMode::NORMAL:
      normalInput(ch);
      break;
    case EditorMode::INSERT:
      insertInput(ch);
      break;
    case EditorMode::COMMAND:
      commandInput(ch);
      break;
    case EditorMode::VISUAL:
      visualInput(ch);
      break;
  }
}

void Editor::moveCursorLeft() {
  if (cursorColumn_ != 0) {
    --cursorColIterator_;
    --cursorColumn_;
  }
}

void Editor::moveCursorDown() {
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
}

void Editor::moveCursorUp() {
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
}

void Editor::moveCursorRight() {
  if (cursorColumn_ != size(*cursorLineIterator_) - 1) {
    ++cursorColIterator_;
    ++cursorColumn_;
  }
}

void Editor::normalInput(char c) {
  switch (c) {
    // Navigation Commands

    case 'h':
      moveCursorLeft();
      break;

    case 'j':
      moveCursorDown();
      break;

    case 'k':
      moveCursorUp();
      break;

    case 'l':
      moveCursorRight();
      break;

    case '^':
      cursorColumn_ = 0;
      cursorColIterator_ = begin(*cursorLineIterator_);
      break;

    case '$':
      cursorColumn_ = static_cast<unsigned int>(size(*cursorLineIterator_) - 1);
      cursorColIterator_ = begin(*cursorLineIterator_);
      std::advance(cursorColIterator_, cursorColumn_);
      break;

    // Mode Switching Commands

    case ':':
      // Enter command mode
      mode = EditorMode::COMMAND;
      commandContents_ = "";
      break;

    case 'i':
      // Enter insert mode
      mode = EditorMode::INSERT;
      break;
    
    case 'a':
      // Enter insert mode 1 character after
      mode = EditorMode::INSERT;
      if (cursorColIterator_ != end(*cursorLineIterator_)) {
        ++cursorColIterator_;
        ++cursorColumn_;
      }
      break;

    case 'o':
      // Enter insert mode on a new line
      {
        mode = EditorMode::INSERT;
        auto nextLineIterator = cursorLineIterator_;
        ++nextLineIterator;
        lines_.emplace(nextLineIterator, std::list<char>());
        ++cursorLineIterator_;
        ++cursorLine_;
        cursorColIterator_ = begin(*cursorLineIterator_);
        cursorColumn_ = 0;
      }
      break;

    case 'O':
      // Enter insert mode one line before
      {
        mode = EditorMode::INSERT;
        lines_.emplace(cursorLineIterator_, std::list<char>());
        --cursorLineIterator_;
        cursorColIterator_ = begin(*cursorLineIterator_);
        cursorColumn_ = 0;
      }
      break;


    case 'v':
      // Enter visual mode at the current position
      mode = EditorMode::VISUAL;
      visualStartLineIterator_ = cursorLineIterator_;
      visualStartColIterator_ = cursorColIterator_;
      break;

    // In-Place Editing Commands

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
          if (newColIterator != begin(*cursorLineIterator_)) {
            --cursorColumn_;
          }
        }
        cursorLineIterator_->erase(cursorColIterator_);
        cursorColIterator_ = newColIterator;
      }
      break;
  }
}

void Editor::insertInput(char c) {
  if (c == '\33') {
    // ESC = exit insert mode
    mode = EditorMode::NORMAL;
    // if we are in a one past the end state, reset to end of line
    if (cursorColIterator_ == end(*cursorLineIterator_)) {
      if (cursorColumn_ != 0) {
        --cursorColumn_;
      }
      cursorColIterator_ = begin(*cursorLineIterator_);
      std::advance(cursorColIterator_, cursorColumn_);
    }
  } else if (c == '\r') {
    // Special case of new line
    auto newLine = std::list<char>();
    auto nextLineIterator = cursorLineIterator_;
    ++nextLineIterator;
    lines_.emplace(nextLineIterator, newLine);

    // move remaining characters
    auto newLineIterator = cursorLineIterator_;
    ++newLineIterator;
    newLineIterator->splice(begin(*newLineIterator), *cursorLineIterator_, cursorColIterator_, end(*cursorLineIterator_));

    // set cursor position to beginning of new line.
    cursorLineIterator_ = newLineIterator;
    cursorColIterator_ = begin(*cursorLineIterator_);
    cursorColumn_ = 0;
    ++cursorLine_;
  } else if (c == '\x7f') {
    // Backspace
    if (cursorColIterator_ == begin(*cursorLineIterator_)) {
      // If at beginning of line or on empty line, merge line with previous line.
      auto prevLine = cursorLineIterator_;
      --prevLine;
      // if previous line is empty, just wipe it lol
      if (size(*prevLine) == 0) {
        lines_.erase(prevLine);
        --cursorLine_;
      } else {
        // fix these beforehand
        --cursorLine_;
        cursorColumn_ = static_cast<unsigned int>(size(*prevLine));
        prevLine->splice(end(*prevLine), *cursorLineIterator_);
        // get rid of the current line
        auto deleteLine = cursorLineIterator_;
        --cursorLineIterator_;
        cursorColIterator_ = begin(*cursorLineIterator_);
        std::advance(cursorColIterator_, cursorColumn_);
        lines_.erase(deleteLine);
      }
    } else {
      // Else, delete character.
      auto deleteIterator = cursorColIterator_;
      --deleteIterator;
      cursorLineIterator_->erase(deleteIterator);
      --cursorColumn_;
    }
  } else {
    cursorLineIterator_->insert(cursorColIterator_, c);
    ++cursorColumn_;
  }
}

void Editor::commandInput(char c) {
  if (c == '\r') {
    // enter = submit command
    executeCommand();
    if (mode == EditorMode::COMMAND) {
      mode = EditorMode::NORMAL;
    }
  } else if (c == '\x7f') {
    if (size(commandContents_) != 0) {
      commandContents_.pop_back();
    }
  } else {
    commandContents_ += c;
  }
}

void Editor::executeCommand() {
  for (char c : commandContents_) {
    if (c == 'w') {
      // Write
      std::ofstream fout(path_);
      for (auto lit = begin(lines_); lit != end(lines_); ++lit) {
        for (auto cit = begin(*lit); cit != end(*lit); ++cit) {
          fout << *cit;
        }
        fout << "\n";
      }
      fout.flush();
      fout.close();
    } else if (c == 'q') {
      // Quit
      mode = EditorMode::STOPPED;
    }
  }
  commandContents_ = "";
}

void Editor::visualInput(char c) {
  switch (c) {
    case '\33':
      // ESC = exit visual mode
      mode = EditorMode::NORMAL;
      break;
    case 'h':
      // Move left
      moveCursorLeft();
      break;
    case 'j':
      // Move down
      moveCursorDown();
      break;
    case 'k':
      // Move up
      moveCursorUp();
      break;
    case 'l':
      // Move right
      moveCursorRight();
      break;
  }
}

std::vector<std::string> Editor::getUsageHints() const {
  std::vector<std::string> hints;
  switch (mode) {
    case EditorMode::STOPPED:
      return {};
    case EditorMode::NORMAL:
      return {
        "h - move left",
        "j - move down",
        "k - move up",
        "l - move right",
        "^ - go to beginning of line",
        "$ - go to end of line",
        "i - insert character",
        "a - append character",
        "o - add line below",
        "O - add line above",
        ": - enter command mode",
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
  bool isVisual = false;
  bool overlap = (visualStartLineIterator_ == cursorLineIterator_ && visualStartColIterator_ == cursorColIterator_);
  std::vector<std::string> lines;

  unsigned int i = 0;
  unsigned int j = 0;

  for (auto lit = begin(lines_); lit != end(lines_); ++lit, ++i) {
    std::string line = "";
    line += "\33[38;5;243m" + std::to_string(lineNumber) + "\33[0m";
    line += " ";
    line = std::string(paddingWidth - static_cast<unsigned int>(size(std::to_string(lineNumber))) - 1, ' ') + line;
    ++lineNumber;

    j = 0;
    for (auto cit = begin(*lit); cit != end(*lit); ++cit) {
      if (mode == VISUAL && lit == visualStartLineIterator_ && 
        cit == visualStartColIterator_ && !overlap) {
        line += "\33[48;5;243m" + std::string{*cit} + "\33[0m";
        isVisual = !isVisual;
      } else if (lit == cursorLineIterator_ && cit == cursorColIterator_) {
        line += "\33[48;5;243m" + std::string{*cit} + "\33[0m";
        cursorScroll_ = static_cast<unsigned int>(size(lines));
        if (mode == VISUAL && !overlap) {
          isVisual = !isVisual;
        }
      } else if (mode == VISUAL && isVisual) {
        line += "\33[48;5;243m" + std::string{*cit} + "\33[0m";
      } else {
        line += *cit;
      }
      ++j;
      if (j + paddingWidth == textWidth) {
        j = 0;
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
