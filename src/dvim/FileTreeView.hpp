// Copyright 2022 Daniel Liu

// Interface for a file tree view, handling keyboard input.

#ifndef DVIM_FILE_TREE_VIEW_HPP_
#define DVIM_FILE_TREE_VIEW_HPP_

#include <filesystem>
#include <memory>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "FileTree.hpp"

namespace dvim {

class FileTreeView {
 public:
  FileTreeView(const std::filesystem::path &path, dcurses::WindowManager &manager);

  void handleInput(char ch);

  void refresh();
 private:
  std::shared_ptr<dcurses::Window> window_;
  FileTree fileTree_;
};

}

#endif
