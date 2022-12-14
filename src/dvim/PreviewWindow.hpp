// Copyright 2022 Daniel Liu

// Interface for a file preview.

#ifndef DVIM_PREVIEW_WINDOW_HPP_
#define DVIM_PREVIEW_WINDOW_HPP_

#include <filesystem>
#include <memory>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "dvim/TextFileLayout.hpp"

namespace dvim {

/*
 * Interface to display file previews.
 */
class PreviewWindow {
 public:
  PreviewWindow(const std::filesystem::path &path, dcurses::WindowManager &manager);

  /*
   * Destroys the preview window, and removes the corresponding window.
   */
  ~PreviewWindow();

  /*
   * Refreshes the preview window, to update the contents.
   */
  void refresh();

  /*
   * Sets the path to preview.
   */
  void setPath(const std::filesystem::path &path);

 private:
  dcurses::WindowManager &windowManager_;
  std::shared_ptr<dcurses::Window> window_;
  std::filesystem::path path_;
};

}

#endif
