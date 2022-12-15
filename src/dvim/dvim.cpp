// Copyright 2022 Daniel Liu

// dvim controller.

#include "dvim.hpp"

#include <iostream>
#include <memory>
#include <stdio.h>

#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"
#include "FileTreeView.hpp"
#include "UsageHintView.hpp"
#include "PreviewWindow.hpp"

namespace dvim {

dvimController::dvimController() : 
  manager_{}, ftv_{".", manager_}, uhv_{manager_}, 
  pw_{std::make_unique<dvim::PreviewWindow>(std::filesystem::path{"text.txt"}, manager_)} {
  uhv_.setHints(std::vector<std::string>{
    " j - down",
    " k - up",
    " SPACE - open directory",
    " ENTER - open file",
    " q - quit",
  });
}

void dvimController::switchToEditor() {
  pw_.reset();
  ev_ = std::make_unique<dvim::EditorView>(ftv_.getSelectedPath(), manager_, *this);
  state = dvimState::EDITOR;
}

void dvimController::switchToPreview() {
  ev_.reset();
  pw_ = std::make_unique<dvim::PreviewWindow>(ftv_.getSelectedPath(), manager_);
  state = dvimState::PREVIEW;
}

void dvimController::run() {
  while (true) {
    if (state == dvimState::PREVIEW) {
      pw_->setPath(ftv_.getSelectedPath());
      pw_->refresh();
    } else if (state == dvimState::EDITOR) {
      ev_->refresh();
      uhv_.setHints(ev_->getUsageHints());
    }
    ftv_.refresh();
    uhv_.refresh();
    manager_.refresh();
    char ch = (char) getc(stdin);
    if (state == dvimState::PREVIEW) {
      if (ch == 'q') {
        break;
      } else if (ch == '\r') {
        // move to editor
        switchToEditor();
        manager_.refresh();
      }
    }
    if (state == dvimState::PREVIEW) {
      ftv_.handleInput(ch);
    } else if (state == dvimState::EDITOR) {
      ev_->handleInput(ch);
    }
  }
}

}
