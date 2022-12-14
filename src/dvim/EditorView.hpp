// Copyright 2022 Daniel Liu

// Interface between the Editor and the TUI.

#ifndef DVIM_EDITOR_VIEW_HPP_
#define DVIM_EDITOR_VIEW_HPP_

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "Editor.hpp"

namespace dvim {

// Forward declaration
class dvimController;

/*
 * Interface between the Editor and the TUI.
 */
class EditorView {
 public:
  /*
   * Constructs the editor view in the specified window manager, using the
   * provided path.
   */
  EditorView(const std::filesystem::path &path, dcurses::WindowManager &manager,
             dvim::dvimController& controller);

  /*
   * Destroys the editor view, and removes the corresponding window.
   */
  ~EditorView();

  /*
   * Handles a single character input action. 
   */
  void handleInput(char ch);

  /*
   * Get the usage hints for the current mode.
   */
  std::vector<std::string> getUsageHints() const { return editor_.getUsageHints(); }

  /*
   * Refreshes the editor view, to update the contents.
   */
  void refresh();

 private:
  dcurses::WindowManager &windowManager_;
  std::shared_ptr<dcurses::Window> window_;
  std::shared_ptr<dcurses::Window> commandWindow_;
  dvim::dvimController& controller_;
  Editor editor_;

  std::filesystem::path path_;
  unsigned int scroll_ = 0;
};

}

#endif
