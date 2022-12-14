// Copyright 2022 Daniel Liu

// Interface for a file tree view, handling keyboard input.

#include "FileTreeView.hpp"

#include <filesystem>
#include <memory>
#include <string>

#include "dcurses/Window.hpp"
#include "FileTree.hpp"
#include "Utilities.hpp"

namespace dvim {

FileTreeView::FileTreeView(const std::filesystem::path &path, dcurses::WindowManager &manager) : 
  windowManager_(manager), fileTree_(path) {
  manager.addWindow("fileTree", {0, 0, 30, manager.getHeight() - 10, 0, DEFAULT_BORDER});
  window_ = manager["fileTree"];
  heightAvailable_ = window_->height() - 2;
}

FileTreeView::~FileTreeView() {
  windowManager_.removeWindow("fileTree");
}

void FileTreeView::handleInput(char ch) {
  if (ch == 'j') {
    if (cursor_ < size(fileTree_.toString()) - 1) {
      cursor_++;
      if (cursor_ >= scroll_ + heightAvailable_) {
        scroll_++;
      }
    }
  } else if (ch == 'k') {
    if (cursor_ > 0) {
      cursor_--;
      if (cursor_ < scroll_) {
        scroll_--;
      }
    }
  } else if (ch == ' ') {
    fileTree_.toggle(fileTree_.lineToPath(cursor_));
  }
}

void FileTreeView::refresh() {
  window_->clear();
  unsigned int row = 1;
  auto str = fileTree_.toString();
  for (unsigned int i = 0; i < heightAvailable_; i++) {
    if (i >= size(str)) break;
    auto line = str[i + scroll_];
    unsigned int col = 2;
    for (auto c : dvim::splitVisibleCharacters(line)) {
      if (i + scroll_ == cursor_ ) {
        window_->setString(row, col, std::string{"\033[48;5;243m"} + c + std::string{"\033[0m"});
      } else {
        window_->setString(row, col, c);
      }
      col++;
      if (col == 28) break;
    }
    row++;
  }
}

}
