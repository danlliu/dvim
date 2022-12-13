
#include "dcurses/Window.hpp"
#include "dcurses/WindowManager.hpp"

#include "dvim/FileTreeView.hpp"
#include "dvim/UsageHintView.hpp"

#include "Utilities.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

class RawSTTY {
 public:
  RawSTTY() {
    FILE* saved_stty = popen("stty -g", "r");
    char buf[1024];
    fgets(buf, 1024, saved_stty);
    saved = std::string(buf);
    system("stty raw");
  }
  ~RawSTTY() {
    system((std::string("stty ") + saved).c_str());
  }
 private:
  std::string saved;
};

int main() {
  RawSTTY stty;

  // std::ifstream img("rickroll.png", std::ios::binary);
  // std::stringstream contents;
  // contents << img.rdbuf();
  // std::string img_contents = contents.str();

  dcurses::WindowManager manager;
  std::cout << manager.getWidth() << "x" << manager.getHeight() << std::endl;

  dvim::FileTreeView ftv(".", manager);
  dvim::UsageHintView uhv(manager);
  // manager.addWindow("window1", { 0, 0, 30, manager.getHeight(), 0, DEFAULT_BORDER });

  // dvim::FileTree tree(".");
  // tree.open(".");
  // tree.open("./src");
  // tree.open("./src/dcurses");

  ftv.refresh();
  uhv.refresh();
  manager.refresh();
  
  getc(stdin);

  uhv.setHints(std::vector<std::string>{
    " h - Move left",
    " j - Move down",
    " k - Move up",
    " l - Move right",
    " q - Quit",
  });
  uhv.refresh();
  manager.refresh();

  getc(stdin);

}
