// Copyright 2022 Daniel Liu

// Interface for a usage hint panel view.

#include "UsageHintView.hpp"

#include <memory>
#include <string>
#include <vector>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "UsageHint.hpp"

namespace dvim {

UsageHintView::UsageHintView(dcurses::WindowManager &manager) : windowManager_(manager), hints_ {} {
  manager.addWindow("usageHints", {manager.getHeight() - 10, 0, manager.getWidth(), 10, 0, DEFAULT_BORDER});
  window_ = manager["usageHints"];
}

UsageHintView::~UsageHintView() {
  windowManager_.removeWindow("usageHints");
}

void UsageHintView::refresh() {
  window_->clear();
  unsigned int row = 1;
  for (auto line : layoutUsageHints(hints_, window_->width() - 4)) {
    unsigned int col = 2;
    window_->setString(row, col, line);
    row++;
  }
}

void UsageHintView::setHints(const std::vector<std::string> &hints) {
  hints_ = hints;
}

}
