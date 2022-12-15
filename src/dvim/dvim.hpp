// Copyright 2022 Daniel Liu

// dvim controller.

#ifndef DVIM_DVIM_HPP_
#define DVIM_DVIM_HPP_

#include <memory>

#include "FileTreeView.hpp"
#include "UsageHintView.hpp"
#include "PreviewWindow.hpp"
#include "EditorView.hpp"

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

  /*
   * Switch to the EDITOR state.
   */
  void switchToEditor();

  /*
   * Switch back to PREVIEW state.
   */
  void switchToPreview();

 private:
  enum dvimState {
    PREVIEW,
    EDITOR
  };
  dvimState state = dvimState::PREVIEW;

  dcurses::WindowManager manager_;
  dvim::FileTreeView ftv_;
  dvim::UsageHintView uhv_;
  std::unique_ptr<dvim::PreviewWindow> pw_;
  std::unique_ptr<dvim::EditorView> ev_;
};

}

#endif
