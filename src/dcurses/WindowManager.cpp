// Copyright 2022 Daniel Liu

// WindowManager.cpp

#include "WindowManager.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <type_traits>
#include <vector>

#include <sys/ioctl.h>
#include <unistd.h>

#include "Logging.hpp"

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
  LOG("Added window: " + name + " with width: " + std::to_string(settings.width) + " and height: " + std::to_string(settings.height));
  std::shared_ptr<Window> window = std::make_shared<Window>(
    settings.row, settings.col, settings.width, settings.height, settings.zIndex, settings.border);
  windows_[nextId_] = window;
  windowsByName_[name] = nextId_;
  ++nextId_;
}

void WindowManager::removeWindow(const std::string &name) {
  windows_.erase(windowsByName_[name]);
  windowsByName_.erase(name);
  for (auto &[id, window] : windows_) {
    window->clearCache();
  }
  std::cout << ESC << "[2J" << std::flush;
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
  
  // std::cout << ESC << "[2J" << std::flush;
  std::cout << ESC << "[3J";

  // ESC[H move cursor to top left.
  std::cout << ESC << "[H";

  // Hide cursor.
  std::cout << ESC << "[?25l";

  // Draw windows.
  while (!windowQueue.empty()) {
    LOG("Drawing window: " + std::to_string(windowQueue.top()));
    std::shared_ptr<Window> window = windows_[windowQueue.top()];
    window->refresh();
    windowQueue.pop();
  }
}

}  // namespace dcurses
