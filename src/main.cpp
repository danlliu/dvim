// Copyright 2022 Daniel Liu

#include "dvim/dvim.hpp"

#include "dvim/TextFileLayout.hpp"

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
    system("tput smcup");
    std::cout << "\33[?47h" << std::flush;
    FILE* saved_stty = popen("stty -g", "r");
    char buf[1024];
    fgets(buf, 1024, saved_stty);
    saved = std::string(buf);
    system("stty raw");
  }
  ~RawSTTY() {
    system((std::string("stty ") + saved).c_str());
    // Restore the screen
    system("tput rmcup");
  }
 private:
  std::string saved;
};

int main() {
  RawSTTY stty;

  std::ifstream img("rickroll.png", std::ios::binary);
  std::stringstream contents;
  contents << img.rdbuf();
  std::string img_contents = contents.str();

  dvim::dvimController controller;
  controller.run();
}
