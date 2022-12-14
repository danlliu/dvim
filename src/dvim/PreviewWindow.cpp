// Copyright 2022 Daniel Liu

// Interface for a file preview.

#include "PreviewWindow.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "dvim/TextFileLayout.hpp"

namespace dvim {

PreviewWindow::PreviewWindow(const std::filesystem::path &path, dcurses::WindowManager &manager)
    : windowManager_(manager), path_(path) {
  manager.addWindow("preview", {0, 30, manager.getWidth() - 30, manager.getHeight() - 10, 0, DOUBLE_BORDER});
  window_ = manager["preview"];
}

PreviewWindow::~PreviewWindow() {
  windowManager_.removeWindow("preview");
}

void PreviewWindow::refresh() {
  window_->clear();
  if (std::filesystem::is_regular_file(path_) == false) {
    return;
  }
  std::ifstream fs(path_, std::ios::binary);
  std::stringstream contentstream;
  contentstream << fs.rdbuf();
  std::string contents = contentstream.str();

  // rough heuristic for ASCII vs binary
  bool isBinary = !std::all_of(contents.begin(), contents.end(), [](char c) { return c >= 0; });

  std::string title = " " + path_.filename().string() + " (preview) ";
  window_->setString(0, 2, title);

  if (isBinary) {
    window_->setString(2, 2, "Binary file");
    return;
  } else {
    auto layout = layoutFileWithLineNums(contents, window_->width() - 4);
    for (unsigned int row = 1; row < window_->height() - 1; ++row) {
      if (row >= size(layout)) break;
      unsigned int col = 2;
      window_->setString(row, col, layout[row - 1]);
    }
  }
}

void PreviewWindow::setPath(const std::filesystem::path &path) {
  path_ = path;
}

}
