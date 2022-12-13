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

class UsageHintView {
 public:
  UsageHintView(dcurses::WindowManager &manager);

  void handleInput(char ch);

  void refresh();

  void setHints(const std::vector<std::string> &hints);
 private:
  std::shared_ptr<dcurses::Window> window_;
  std::vector<std::string> hints_;
};

}

#endif
