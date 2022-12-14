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

/*
 * Interface between FileTree and the TUI.
 */
class FileTreeView {
 public:
  /*
   * Constructs the file tree view in the specified window manager.
   */
  FileTreeView(const std::filesystem::path &path, dcurses::WindowManager &manager);

  /*
   * Destroys the file tree view, and removes the corresponding window.
   */
  ~FileTreeView();

  /*
   * Handles a single character input action.
   */
  void handleInput(char ch);

  /*
   * Refreshes the file tree view, to update the contents.
   */
  void refresh();

  /*
   * Get the currently selected path.
   */
  std::filesystem::path getSelectedPath() const { return fileTree_.lineToPath(cursor_); };

 private:
  dcurses::WindowManager &windowManager_;
  std::shared_ptr<dcurses::Window> window_;
  FileTree fileTree_;

  unsigned int cursor_ = 0;
  unsigned int scroll_ = 0;
  unsigned int heightAvailable_ = 0;
};

}

#endif
