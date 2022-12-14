// Copyright 2022 Daniel Liu

// Interface for a usage hint panel view.

#ifndef DVIM_USAGE_HINT_VIEW_HPP_
#define DVIM_USAGE_HINT_VIEW_HPP_

#include <memory>
#include <string>
#include <vector>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "UsageHint.hpp"

namespace dvim {

/*
 * Interface between UsageHint and the TUI.
 */
class UsageHintView {
 public:
  /*
   * Initializes the usage hint view in the specified window manager.
   */
  UsageHintView(dcurses::WindowManager &manager);

  /*
   * Destroys the usage hint view, and removes the corresponding window.
   */
  ~UsageHintView();

  /*
   * Refreshes the usage hint view, to update the contents.
   */
  void refresh();

  /*
   * Sets the hints that are shown.
   */
  void setHints(const std::vector<std::string> &hints);

 private:
  dcurses::WindowManager &windowManager_;
  std::shared_ptr<dcurses::Window> window_;
  std::vector<std::string> hints_;
};

}

#endif
