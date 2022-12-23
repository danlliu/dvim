// Copyright 2022 Daniel Liu

// Object for representing an editor.

#include <array>
#include <filesystem>
#include <list>
#include <string>
#include <vector>

#include "dcurses/WindowManager.hpp"

#define NUM_REGS 10

#ifndef DVIM_EDITOR_HPP_
#define DVIM_EDITOR_HPP_

namespace dvim {

class Editor {
 public:
  /*
   * Initialize the editor with the specified file path and window manager.
   */
  Editor(const std::filesystem::path &path, dcurses::WindowManager &manager);

  /*
   * Handle a single character input action.
   */
  void handleInput(char ch);

  /*
   * Get the usage hints for the current mode.
   */
  std::vector<std::string> getUsageHints() const;

  /*
   * Returns a list of the current lines, with the cursor highlighted.
   */
  std::vector<std::string> getLines(unsigned int width);

  /*
   * Returns the current cursor line.
   */
  unsigned int getCursorLine() const { return cursorLine_; }

  /*
   * Returns the current cursor column.
   */
  unsigned int getCursorColumn() const { return cursorColumn_; }

  /*
   * Returns the line that the cursor is on in the most recent call to getLines.
   */
  unsigned int getCursorScroll() const { return cursorScroll_; }

  /*
   * Returns the queued actions currently.
   */
  std::string getQueuedActions() const { return queuedActions_; }

  /*
   * Returns the current command contents.
   */
  std::string getCommandContents() const { return queuedActions_; }

  /*
   * Returns the current mode.
   */
  std::string getMode() const {
    switch (mode) {
      case EditorMode::STOPPED:
        return "STOPPED";
      case EditorMode::NORMAL:
        return "NORMAL";
      case EditorMode::INSERT:
        return "INSERT";
      case EditorMode::COMMAND:
        return "COMMAND";
      case EditorMode::VISUAL:
        return "VISUAL";
      case EditorMode::REGWINDOW:
        return "REG SHOW";
      default:
        return "UNKNOWN";
    }
  }

 private:
  enum EditorMode {
    STOPPED,
    NORMAL,
    INSERT,
    COMMAND,
    VISUAL,
    REGWINDOW
  };

  void normalInput(char c);
  void insertInput(char c);
  void commandInput(char c);
  void visualInput(char c);
  void regWindowInput(char c);

  // Common movement
  void moveCursorLeft();
  void moveCursorRight();
  void moveCursorUp();
  void moveCursorDown();

  bool isSingleNavigationAction(char c);
  bool isSingleInPlaceEditAction(char c);
  bool isQueueableNormalAction(char c);
  void executeNormalAction(char c);
  void executeDeleteAction(char c);
  void executeCommand();

  EditorMode mode = EditorMode::NORMAL;

  dcurses::WindowManager &manager_;

  std::filesystem::path path_;
  std::list<std::list<char>> lines_;
  unsigned int cursorLine_ = 0;
  unsigned int cursorColumn_ = 0;
  unsigned int cursorScroll_ = 0;

  // Invariants:
  // If NORMAL, COMMAND, or VISUAL mode:
  // - cursorLineIterator_ refers to the line that the cursor is on.
  //   cursorLineIterator_ may be an end iterator if and only if there are no lines.
  // - cursorColIterator_ refers to the column that the cursor is on.
  //   cursorColIterator_ may be an end iterator if and only if the line has no content.
  // If INSERT mode:
  // - cursorLineIterator_ refers to the line that the cursor is on.
  //   cursorLineIterator_ may be an end iterator if and only if there are no lines.
  // - cursorColIterator_ refers to the column that the cursor is on.
  //   cursorColIterator_ may be an end iterator if and only if the cursor is at the end of the line (past the last character).
  std::list<std::list<char>>::iterator cursorLineIterator_;
  std::list<char>::iterator cursorColIterator_;

  // Editor variables

  std::array<std::string, NUM_REGS> registers_;
  unsigned int activeRegister_ = 0;

  // Mode-specific variables

  unsigned int visualStartLine_ = 0;
  unsigned int visualStartColumn_ = 0;

  std::list<std::list<char>>::iterator visualStartLineIterator_;
  std::list<char>::iterator visualStartColIterator_;

  std::string queuedActions_ = "";
};

};

#endif
