// Copyright 2022 Daniel Liu

// Interface between the Editor and the TUI.

#include "EditorView.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "dvim.hpp"
#include "Editor.hpp"

namespace dvim {

EditorView::EditorView(const std::filesystem::path &path, 
  dcurses::WindowManager &manager, dvim::dvimController& controller) 
  : windowManager_(manager), controller_(controller), editor_(path, manager), path_(path) {
  manager.addWindow("editor", {0, 30, manager.getWidth() - 30, manager.getHeight() - 10, 0, DOUBLE_BORDER});
  window_ = manager["editor"];
  manager.addWindow("command", {manager.getHeight() - 1, 0, manager.getWidth(), 1, 0, NO_BORDER});
  commandWindow_ = manager["command"];
}

EditorView::~EditorView() {
  windowManager_.removeWindow("editor");
  windowManager_.removeWindow("command");
}

void EditorView::handleInput(char ch) {
  editor_.handleInput(ch);
  if (editor_.getMode() == "STOPPED") {
    controller_.switchToPreview();
  }
}

void EditorView::refresh() {
  window_->clear();
  auto str = editor_.getLines(window_->width());

  std::string title = " " + path_.filename().string() + " [" + editor_.getMode() + "] ";
  window_->setString(0, 2, title);
  window_->setString(window_->height() - 1, 2, " R" + std::to_string(editor_.getCursorLine()) + ":C" + std::to_string(editor_.getCursorColumn()) + " ");
  
  // Check scroll bounds
  if (editor_.getCursorScroll() < scroll_) {
    scroll_ = editor_.getCursorScroll();
  } else if (editor_.getCursorScroll() >= scroll_ + window_->height() - 2) {
    scroll_ = editor_.getCursorScroll() - (window_->height() - 2) + 1;
  }

  // Draw lines
  for (unsigned int i = 1; i < window_->height() - 1; i++) {
    if (i - 1 + scroll_ >= size(str)) break;
    auto line = str[i - 1 + scroll_];
    unsigned int col = 2;
    window_->setString(i, col, line);
  }

  // Display command if necessary
  commandWindow_->clear();
  if (editor_.getMode() == "NORMAL") {
    commandWindow_->setString(0, 0, "\33[1m" + editor_.getQueuedActions() + "\33[0m");
  } else if (editor_.getMode() == "COMMAND") {
    commandWindow_->setString(0, 0, "\33[1m:" + editor_.getCommandContents() + "\33[0m");
  }
}

}
