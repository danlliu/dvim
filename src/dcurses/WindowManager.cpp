// Copyright 2022 Daniel Liu

// WindowManager.cpp

#include "WindowManager.hpp"

#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <type_traits>
#include <vector>

#include <sys/ioctl.h>
#include <unistd.h>

#define ESC "\33"

namespace dcurses {

WindowManager::WindowManager() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  screenWidth_ = w.ws_col;
  screenHeight_ = w.ws_row;
}

WindowManager::~WindowManager() {
}

void WindowManager::addWindow(const std::string &name, const WindowSettings &settings) {
  std::shared_ptr<Window> window = std::make_shared<Window>(
    settings.row, settings.col, settings.width, settings.height, settings.zIndex, settings.border);
  windows_[nextId_] = window;
  windowsByName_[name] = nextId_;
  ++nextId_;
}

void WindowManager::removeWindow(const std::string &name) {
  windows_.erase(windowsByName_[name]);
  windowsByName_.erase(name);
}

std::shared_ptr<Window> WindowManager::operator[](const std::string &name) {
  auto it = windowsByName_.find(name);
  if (it != windowsByName_.end()) {
    return windows_[it->second];
  }
  return {};
}

void WindowManager::refresh() {
  auto comp = [&](auto window1, auto window2) {
    return windows_[window1]->zIndex() < windows_[window2]->zIndex();
  };

  std::vector<int> ids ( size(windows_), 0 );
  std::transform(windows_.begin(), windows_.end(), ids.begin(), [](auto it) { return it.first; });

  // Sort windows by zIndex.
  std::priority_queue<int, std::vector<int>, decltype(comp)> windowQueue{begin(ids), end(ids), comp};

  // system("clear");
  
  std::cout << ESC << "[2J" << std::flush;
  std::cout << ESC << "[3J" << std::flush;

  // ESC[H move cursor to top left.
  std::cout << ESC << "[H" << std::flush;

  // Hide cursor.
  std::cout << ESC << "[?25l" << std::flush;

  // Draw windows.
  while (!windowQueue.empty()) {
    std::shared_ptr<Window> window = windows_[windowQueue.top()];
    window->refresh();
    windowQueue.pop();
  }
}

}  // namespace dcurses
