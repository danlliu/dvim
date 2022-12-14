// Copyright 2022 Daniel Liu

// dvim controller.

#ifndef DVIM_DVIM_HPP_
#define DVIM_DVIM_HPP_

#include <memory>

#include "FileTreeView.hpp"
#include "UsageHintView.hpp"
#include "PreviewWindow.hpp"

#include "dcurses/WindowManager.hpp"

namespace dvim {

/*
 * The main controller for dvim.
 */
class dvimController {
 public:
  /*
   * Initialize the controller.
   */
  dvimController();

  /*
   * Run dvim.
   */
  void run();
 private:
  dcurses::WindowManager manager_;
  dvim::FileTreeView ftv_;
  dvim::UsageHintView uhv_;
  std::unique_ptr<dvim::PreviewWindow> pw_;
};

}

#endif
