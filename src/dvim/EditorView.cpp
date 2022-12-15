// Copyright 2022 Daniel Liu

// Interface between the Editor and the TUI.

#include "EditorView.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "Editor.hpp"

namespace dvim {

EditorView::EditorView(const std::filesystem::path &path, dcurses::WindowManager &manager) : windowManager_(manager), editor_(path), path_(path) {
  manager.addWindow("editor", {0, 30, manager.getWidth() - 30, manager.getHeight() - 10, 0, DOUBLE_BORDER});
  window_ = manager["editor"];
}

EditorView::~EditorView() {
  windowManager_.removeWindow("editor");
}

void EditorView::handleInput(char ch) {
  editor_.handleInput(ch);
}

void EditorView::refresh() {
  window_->clear();
  auto str = editor_.getLines(window_->width());

  std::string title = " " + path_.filename().string() + " [" + editor_.getMode() + "] ";
  window_->setString(0, 2, title);
  
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
}

}
