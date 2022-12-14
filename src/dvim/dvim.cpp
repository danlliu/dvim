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

void dvimController::run() {
  while (true) {
    ftv_.refresh();
    uhv_.refresh();
    if (state == dvimState::PREVIEW) {
      pw_->setPath(ftv_.getSelectedPath());
      pw_->refresh();
    } else if (state == dvimState::EDITOR) {
      ev_->refresh();
    }
    manager_.refresh();
    char ch = (char) getc(stdin);
    if (ch == 'q') {
      break;
    }
    if (state == dvimState::PREVIEW) {
      if (ch == '\r') {
        // move to editor
        pw_.reset();
        ev_ = std::make_unique<dvim::EditorView>(ftv_.getSelectedPath(), manager_);
        state = dvimState::EDITOR;
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
