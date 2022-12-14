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
    " j     - down",
    " k     - up",
    " SPACE - open directory",
    " ENTER - open file",
    " q     - quit",
  });
}

void dvimController::run() {
  while (true) {
    ftv_.refresh();
    uhv_.refresh();
    if (pw_) {
      pw_->setPath(ftv_.getSelectedPath());
      pw_->refresh();
    }
    manager_.refresh();
    char ch = (char) getc(stdin);
    if (ch == 'q') {
      break;
    }
    ftv_.handleInput(ch);
  }
}

}
