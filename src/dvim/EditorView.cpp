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

EditorView::EditorView(const std::filesystem::path &path, dcurses::WindowManager &manager) : windowManager_(manager), editor_(path) {
  manager.addWindow("editor", {0, 30, manager.getWidth() - 30, manager.getHeight() - 10, 2, DOUBLE_BORDER});
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
  
  // Check scroll bounds
  if (editor_.getCursorScroll() < scroll_) {
    scroll_ = editor_.getCursorScroll();
  } else if (editor_.getCursorScroll() >= scroll_ + window_->height() - 2) {
    scroll_ = editor_.getCursorScroll() - window_->height() - 2;
  }

  // Draw lines
  for (unsigned int i = 0; i < window_->height() - 2; i++) {
    if (i >= size(str)) break;
    auto line = str[i + scroll_];
    unsigned int col = 2;
    window_->setString(i + 1, col, line);
  }
}

}
