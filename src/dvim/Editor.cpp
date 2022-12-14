// Copyright 2022 Daniel Liu

// Object for representing an editor window.

#include "Editor.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

#include "Logging.hpp"
#include "dcurses/Window.hpp"
#include "Utilities.hpp"

namespace dvim {

Editor::Editor(const std::filesystem::path &path,
  dcurses::WindowManager& manager) : manager_(manager), path_(path) {
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
    case EditorMode::ERROR:
      mode = EditorMode::NORMAL;
      break;
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
    case EditorMode::REGWINDOW:
      regWindowInput(ch);
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
  if (queuedActions_ != "") {
    std::smatch match;
    unsigned int repetitions = 1;
    if (std::regex_match(queuedActions_, match, std::regex{"([0-9]+)(.*)"})) {
      repetitions = static_cast<unsigned int>(std::stoul(match[1].str()));
      queuedActions_ = match[2].str();
    }
    for (unsigned int i = 0; i < repetitions; ++i) {
      if (queuedActions_ == "") {
        executeNormalAction(c);
      } else if (queuedActions_ == "d") {
        executeDeleteAction(c);
      }
    }
    queuedActions_ = "";
    return;
  } else if (isSingleNavigationAction(c) || isSingleInPlaceEditAction(c)) {
    executeNormalAction(c);
    return;
  }
  switch (c) {
    // Mode Switching Commands

    case ':':
      // Enter command mode
      mode = EditorMode::COMMAND;
      queuedActions_ = "";
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
      visualStartLine_ = cursorLine_;
      visualStartColumn_ = cursorColumn_;
      visualStartLineIterator_ = cursorLineIterator_;
      visualStartColIterator_ = cursorColIterator_;
      break;
    
    case 'V':
      // Enter visual mode with the current line selected
      mode = EditorMode::VISUAL;
      {
        visualStartLineIterator_ = cursorLineIterator_;
        visualStartColIterator_ = begin(*cursorLineIterator_);
        visualStartLine_ = cursorLine_;
        visualStartColumn_ = 0;

        cursorColumn_ = static_cast<unsigned int>(size(*cursorLineIterator_) - 1);
        cursorColIterator_ = begin(*cursorLineIterator_);
        std::advance(cursorColIterator_, cursorColumn_);
      }
      break;

    default:
      // Potential queued commands
      if (isQueueableNormalAction(c) || (c >= '0' && c <= '9')) {
        queuedActions_ += c;
      }

  }
}

bool Editor::isSingleNavigationAction(char c) {
  switch (c) {
    case 'h':
    case 'j':
    case 'k':
    case 'l':
    case 'w':
    case 'e':
    case 'b':
    case '^':
    case '$':
    case 'p':
      return true;
    default:
      return false;
  }
}

bool Editor::isSingleInPlaceEditAction(char c) {
  switch (c) {
    case 'x':
      return true;
    default:
      return false;
  }
}

bool Editor::isQueueableNormalAction(char c) {
  switch (c) {
    case 'd':
      return true;
    default:
      return false;
  }
}

void Editor::executeNormalAction(char c) {
  switch (c) {
    // Navigation

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

    case 'w':
      // Move to the beginning of the next word
      {
        moveCursorRight();
        auto nextChar = cursorColIterator_;
        ++nextChar;
        while (*cursorColIterator_ != ' ' && nextChar != end(*cursorLineIterator_)) {
          ++cursorColIterator_;
          ++cursorColumn_;
          ++nextChar;
        }
        moveCursorRight();
      }
      break;

    case 'e':
      // Move to the end of the current word
      {
        auto nextChar = cursorColIterator_;
        ++nextChar;
        if (*nextChar == ' ') {
          moveCursorRight();
          ++nextChar;
        }
        while (*nextChar != ' ' && nextChar != end(*cursorLineIterator_)) {
          ++cursorColIterator_;
          ++cursorColumn_;
          ++nextChar;
        }
      }
      break;
    
    case 'b':
      // Move to the beginning of the previous word
      {
        if (cursorColIterator_ == begin(*cursorLineIterator_)) {
          break;
        }
        auto prevChar = cursorColIterator_;
        --prevChar;
        if (*prevChar == ' ') {
          moveCursorLeft();
          --prevChar;
        }
        while (cursorColIterator_ != begin(*cursorLineIterator_) && *prevChar != ' ') {
          moveCursorLeft();
          --prevChar;
        }
      }
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

    // Editing

    case 'x':
      // Delete character at cursor.
      {
        if (cursorColIterator_ == end(*cursorLineIterator_)) {
          break;
        }
        registers_[activeRegister_] = std::string{*cursorColIterator_};
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

    case 'p':
      // Paste register content after cursor.
      {
        auto toPaste = registers_[activeRegister_];
        ++cursorColIterator_;
        for (auto c : toPaste) {
          if (c == '\n') {
            auto nextLineInsert = cursorLineIterator_;
            ++nextLineInsert;
            lines_.emplace(nextLineInsert, std::list<char>());
            auto nextLine = cursorLineIterator_;
            ++nextLine;
            nextLine->splice(
              end(*nextLine), *cursorLineIterator_, 
              cursorColIterator_, end(*cursorLineIterator_));
            ++cursorLineIterator_;
            ++cursorLine_;
            cursorColIterator_ = begin(*cursorLineIterator_);
            cursorColumn_ = 0;
          } else {
            cursorLineIterator_->insert(cursorColIterator_, c);
            ++cursorColumn_;
          }
        }
        if (cursorColIterator_ != begin(*cursorLineIterator_)) {
          --cursorColIterator_;
        }
      }
      break;
  
    default:
      break;
  }
}

void Editor::executeDeleteAction(char c) {
  switch (c) {
    case 'h':
      // Delete previous character
      {
        auto prev = cursorColIterator_;
        --prev;
        if (prev == end(*cursorLineIterator_)) {
          break;
        }
        registers_[activeRegister_] = std::string{*prev};
        cursorLineIterator_->erase(prev);
        --cursorColumn_;
      }
      break;
    case 'j':
      // Delete current line and line below
      {
        auto nextLine = cursorLineIterator_;
        ++nextLine;
        if (nextLine == end(lines_)) {
          break;
        }
        auto newLine = nextLine;
        ++newLine;
        if (newLine == end(lines_)) {
          newLine = cursorLineIterator_;
          --newLine;
          --cursorLine_;
        }
        std::string toCopy = "";
        toCopy += std::string(begin(*cursorLineIterator_), end(*cursorLineIterator_));
        toCopy += "\n";
        toCopy += std::string(begin(*nextLine), end(*nextLine));
        toCopy += "\n";
        registers_[activeRegister_] = toCopy;
        lines_.erase(cursorLineIterator_);
        lines_.erase(nextLine);
        cursorLineIterator_ = newLine;
        cursorColIterator_ = begin(*cursorLineIterator_);
        cursorColumn_ = 0;
      }
      break;
    case 'k':
      // Delete current line and line above
      {
        auto prevLine = cursorLineIterator_;
        --prevLine;
        if (prevLine == end(lines_)) {
          break;
        }
        auto nextLine = cursorLineIterator_;
        ++nextLine;
        if (nextLine == end(lines_)) {
          nextLine = prevLine;
          --cursorLine_;
          --nextLine;
        }
        std::string toCopy = "";
        toCopy += std::string(begin(*prevLine), end(*prevLine));
        toCopy += "\n";
        toCopy += std::string(begin(*cursorLineIterator_), end(*cursorLineIterator_));
        toCopy += "\n";
        registers_[activeRegister_] = toCopy;
        lines_.erase(cursorLineIterator_);
        lines_.erase(prevLine);
        cursorLineIterator_ = nextLine;
        cursorColIterator_ = begin(*cursorLineIterator_);
        --cursorLine_;
        cursorColumn_ = 0;
      }
      break;
    case 'l':
      // Delete current character
      {
        auto next = cursorColIterator_;
        ++next;
        if (next == end(*cursorLineIterator_)) {
          next = cursorColIterator_;
          --next;
          --cursorColumn_;
        }
        registers_[activeRegister_] = std::string{*cursorColIterator_};
        cursorLineIterator_->erase(cursorColIterator_);
        cursorColIterator_ = next;
      }
      break;
    case 'w':
      // Delete until a space has been deleted
      {
        bool spaceDeleted = false;
        std::string toCopy = "";
        while (!spaceDeleted && cursorColIterator_ != end(*cursorLineIterator_)) {
          auto next = cursorColIterator_;
          ++next;
          toCopy += std::string{*cursorColIterator_};
          spaceDeleted = *cursorColIterator_ == ' ';
          cursorLineIterator_->erase(cursorColIterator_);
          cursorColIterator_ = next;
        }
        registers_[activeRegister_] = toCopy;
      }
      break;
    case 'e':
      // Delete until some characters have been deleted and a space is reached
      {
        bool nonSpaceDeleted = false;
        std::string toCopy = "";
        while (!(nonSpaceDeleted && *cursorColIterator_ == ' ') && 
          cursorColIterator_ != end(*cursorLineIterator_)) {
          auto next = cursorColIterator_;
          ++next;
          toCopy += std::string{*cursorColIterator_};
          nonSpaceDeleted = *cursorColIterator_ != ' ';
          cursorLineIterator_->erase(cursorColIterator_);
          cursorColIterator_ = next;
        }
        registers_[activeRegister_] = toCopy;
      }
      break;
    case 'b':
      // Delete previous characters until space is reached
      {
        if (cursorColIterator_ == begin(*cursorLineIterator_)) {
          break;
        }
        --cursorColIterator_;
        bool nonSpaceDeleted = false;
        std::string toCopy = "";
        while (!(nonSpaceDeleted && *cursorColIterator_ == ' ') && 
          cursorColIterator_ != end(*cursorLineIterator_)) {
          auto prev = cursorColIterator_;
          --prev;
          --cursorColumn_;
          toCopy += std::string{*cursorColIterator_};
          nonSpaceDeleted = *cursorColIterator_ != ' ';
          cursorLineIterator_->erase(cursorColIterator_);
          cursorColIterator_ = prev;
        }
        ++cursorColIterator_;
        toCopy = std::string{rbegin(toCopy), rend(toCopy)};
        registers_[activeRegister_] = toCopy;
      }
      break;
    default:
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
  if (c == '\33') {
    // ESC = exit command mode
    mode = EditorMode::NORMAL;
  } else if (c == '\r') {
    // enter = submit command
    executeCommand();
    if (mode == EditorMode::COMMAND) {
      mode = EditorMode::NORMAL;
    }
  } else if (c == '\x7f') {
    if (size(queuedActions_) != 0) {
      queuedActions_.pop_back();
    }
  } else {
    queuedActions_ += c;
  }
}

void Editor::executeCommand() {
  if (queuedActions_ == "reg show") {
    // Open register window
    mode = EditorMode::REGWINDOW;
    auto editor = manager_["editor"];
    manager_.addWindow("registers",
      { editor->row() + 4, editor->col() + 8, editor->width() - 16, editor->height() - 8, 4, DEFAULT_BORDER}
    );
    auto window = manager_["registers"];

    window->setString(2, 3, "Registers");
    window->setString(3, 3, "=========");
    for (unsigned int i = 0; i < NUM_REGS; ++i) {
      if (i == activeRegister_) {
        window->setString(4 + i, 3, std::to_string(i) + "*: " + dvim::escapeString(registers_[i]));
      } else {
        window->setString(4 + i, 3, std::to_string(i) + " : " + dvim::escapeString(registers_[i]));
      }
    }
  } else if (std::regex_match(queuedActions_, std::regex{"reg select ([0-9]+)"})) {
    // Select register
    std::smatch matchResult;
    std::regex_match(queuedActions_, matchResult, 
      std::regex{"reg select ([0-9]+)"});
    unsigned int newRegister = static_cast<unsigned int>(std::stoul(matchResult[1].str()));
    if (newRegister < NUM_REGS) {
      activeRegister_ = newRegister;
    }
  } else if (std::regex_match(queuedActions_, std::regex{"[wq]+"})) {
    for (char c : queuedActions_) {
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
  } else {
    errorMessage_ = "Unrecognized command " + std::string{queuedActions_};
    mode = EditorMode::ERROR;
  }
  queuedActions_ = "";
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
    case 'y':
      // Copy selection
      {
        std::string copyString;
        bool forward = (cursorLine_ == visualStartLine_) ?
          (cursorColumn_ > visualStartColumn_) :
          (cursorLine_ > visualStartLine_);
        auto lit = forward ? visualStartLineIterator_ : cursorLineIterator_;
        auto cit = forward ? visualStartColIterator_ : cursorColIterator_;
        auto elit = forward ? cursorLineIterator_ : visualStartLineIterator_;
        auto ecit = forward ? cursorColIterator_ : visualStartColIterator_;

        while (lit != elit) {
          for (; cit != end(*lit); ++cit) {
            copyString += *cit;
          }
          copyString += '\n';
          ++lit;
          cit = begin(*lit);
        }
        for (; cit != ecit; ++cit) {
          copyString += *cit;
        }
        copyString += *cit;
        registers_[activeRegister_] = copyString;
        mode = EditorMode::NORMAL;
      }
      break;
  }
}

void Editor::regWindowInput(char c) {
  switch (c) {
    case '\33':
      // ESC = exit register window
      mode = EditorMode::NORMAL;
      manager_.removeWindow("registers");
      break;
    default:
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
        "w - beginning of next word",
        "e - end of next word",
        "b - beginning of previous word",
        "^ - go to beginning of line",
        "$ - go to end of line",
        "i - insert character",
        "a - append character",
        "o - add line below",
        "O - add line above",
        ": - enter command mode",
        "v - enter visual mode",
        "x - delete character",
        "p - paste contents of active register"
      };
    case EditorMode::INSERT:
      return {
        "ESC - exit insert mode"
      };
    case EditorMode::COMMAND:
      return {
        "ESC - exit command mode",
        "ENTER - submit command",
        "w - save file",
        "q - quit editor",
        "reg show - show register contents",
        "reg select <x> - select register x"
      };
    case EditorMode::VISUAL:
      return {
        "ESC - exit visual mode",
        "h - move left",
        "j - move down",
        "k - move up",
        "l - move right",
        "y - copy selection to active register"
      };
    case EditorMode::REGWINDOW:
      return {
        "ESC - exit register window"
      };
    case EditorMode::ERROR:
      return {
        "any key - exit error mode"
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
