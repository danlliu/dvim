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

FileTreeView::FileTreeView(const std::filesystem::path &path, dcurses::WindowManager &manager) : fileTree_(path) {
  manager.addWindow("fileTree", {0, 0, 30, manager.getHeight() - 10, 0, DEFAULT_BORDER});
  window_ = manager["fileTree"];
}

void FileTreeView::refresh() {
  window_->clear();
  unsigned int row = 1;
  for (auto line : fileTree_.toString()) {
    unsigned int col = 2;
    for (auto c : dvim::splitVisibleCharacters(line)) {
      window_->setString(row, col, c);
      col++;
      if (col == 28) break;
    }
    row++;
  }
}

}
