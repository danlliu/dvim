// Copyright 2022 Daniel Liu

// WindowManager.hpp
// C++ API for ncurses-like window management.

#ifndef DCURSES_WINDOW_MANAGER_HPP_
#define DCURSES_WINDOW_MANAGER_HPP_

#include <map>
#include <memory>
#include <string>

#include "Window.hpp"

namespace dcurses {

/*
 * Class to manage and display multiple windows.
 */
class WindowManager {
 public:
  /*
   * A POD struct containing information about a window.
   */
  struct WindowSettings {
    unsigned int row;
    unsigned int col;
    unsigned int width;
    unsigned int height;
    int zIndex;
    Window::WindowBorder border;
  };

  /*
   * Construct a new WindowManager. Automatically detects screen width and height.
   */
  WindowManager();

  /*
   * Deleted copy and move constructors/assignment operators.
   */
  WindowManager(const WindowManager& other) = delete;
  WindowManager& operator=(const WindowManager& other) = delete;
  WindowManager(const WindowManager&& other) = delete;
  WindowManager& operator=(const WindowManager&& other) = delete;

  /*
   * Destructor. Does nothing.
   */
  ~WindowManager();

  /*
   * Adds a new window to the window manager.
   * Window parameters are set using the WindowSettings struct.
   * @param name The name of the window.
   * @param settings The settings of the window.
   */
  void addWindow(const std::string &name, const WindowSettings &settings);

  /*
   * Remove a window from the window manager.
   * @param name The name of the window to remove.
   */
  void removeWindow(const std::string &name);

  /*
   * Accesses a window by name.
   * @param name The name of the window.
   * @return A weak pointer to the window.
   */
  std::shared_ptr<Window> operator[](const std::string &name);

  /*
   * Refreshes the window, displaying it on the terminal.
   */
  void refresh();

  /*
   * Returns the height of the window.
   */
  unsigned int getHeight() const { return screenHeight_; }

  /*
   * Returns the width of the window.
   */
  unsigned int getWidth() const { return screenWidth_; }

 private:
  unsigned int screenWidth_;
  unsigned int screenHeight_;

  int nextId_ = 0;
  std::map<int, std::shared_ptr<Window>> windows_;
  std::map<std::string, int> windowsByName_; // name -> id
};

}  // namespace dcurses

#endif
