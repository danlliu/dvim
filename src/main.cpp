// Copyright 2022 Daniel Liu

#include "dvim/dvim.hpp"
#include "dcurses/Base64.hpp"

#include "dvim/Editor.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

class RawSTTY {
 public:
  RawSTTY() {
    // Save the screen.
    if (system("tput smcup")) {
      std::cout << "tput smcup failed" << std::endl;
      exit(1);
    }
    std::cout << "\33[?47h" << std::flush;
    FILE* saved_stty = popen("stty -g", "r");
    char buf[1024];
    saved_ = std::string(fgets(buf, 1024, saved_stty));
    if (system("stty raw")) {
      std::cout << "stty raw failed" << std::endl;
      exit(1);
    }
  }
  ~RawSTTY() {
    if (system((std::string("stty ") + saved_).c_str())) {
      std::cout << "stty restore failed" << std::endl;
      exit(1);
    }
    // Restore the screen
    if (system("tput rmcup")) {
      std::cout << "tput rmcup failed" << std::endl;
      exit(1);
    }
  }
 private:
  std::string saved_;
};

int main() {
  RawSTTY stty;

  // iterm2 detection
  FILE* fp = popen("echo $LC_TERMINAL", "r");
  char value[2];
  dcurses::Window::setIterm2(fread(&value, 1, 2, fp) == 2);

  dvim::dvimController controller;
  controller.run();
}
