// Copyright 2022 Daniel Liu

// Window.cpp

#include "Window.hpp"

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "Base64.hpp"

#define ESC "\33"

namespace dcurses {

Window::Window(
  unsigned int row, unsigned int col, unsigned int width, unsigned int height, 
  int zIndex, const WindowBorder &border) :
  row_ {row}, col_ {col}, width_ {width}, height_ {height}, zIndex_ {zIndex}, border_ {border} {
  for (unsigned int i = 0; i < width_; ++i) {
    setString(0, i, border_.top_);
    setString(height_ - 1, i, border_.bottom_);
  }
  for (unsigned int i = 0; i < height_; ++i) {
    setString(i, 0, border_.left_);
    setString(i, width_ - 1, border_.right_);
  }
  setString(0, 0, border_.top_left_);
  setString(0, width_ - 1, border_.top_right_);
  setString(height_ - 1, 0, border_.bottom_left_);
  setString(height_ - 1, width_ - 1, border_.bottom_right_);
}

void Window::setCharacter(unsigned int row, unsigned int col, char character) {
  directions_.emplace_back(RenderDirection{ row, col, character });
}

void Window::setString(unsigned int row, unsigned int col, const std::string& string) {
  directions_.emplace_back(RenderDirection{ row, col, string });
}

void Window::drawImage(unsigned int row, unsigned int col, const ImageContent &image) {
  directions_.emplace_back(RenderDirection{ row, col, image });
}

void Window::refresh() {
  for (const auto &direction : directions_) {
    unsigned int row = direction.row;
    unsigned int col = direction.col;
    unsigned int line = row_ + row + 1;
    unsigned int column = col_ + col + 1;
    std::cout << ESC << "[" << line << ";" << column << "f";
    if (std::holds_alternative<char>(direction.content)) {
      const auto &character = std::get<char>(direction.content);
      std::cout << character;
    } else if (std::holds_alternative<std::string>(direction.content)) {
      const auto &string = std::get<std::string>(direction.content);
      std::cout << string;
    } else {
      const auto &image = std::get<ImageContent>(direction.content);
      std::cout << ESC << "]1337;File=inline=1;size=" << size(image.content_);
      std::cout << ";width=" << image.width_ << ";height=" << image.height_;
      std::cout << ":" << base64Encode(image.content_) << "\a";
    }
  }
  std::cout << std::flush;
}

void Window::clear() {
  directions_.clear();
  for (unsigned int i = 0; i < width_; ++i) {
    setString(0, i, border_.top_);
    setString(height_ - 1, i, border_.bottom_);
  }
  for (unsigned int i = 0; i < height_; ++i) {
    setString(i, 0, border_.left_);
    setString(i, width_ - 1, border_.right_);
  }
  setString(0, 0, border_.top_left_);
  setString(0, width_ - 1, border_.top_right_);
  setString(height_ - 1, 0, border_.bottom_left_);
  setString(height_ - 1, width_ - 1, border_.bottom_right_);
}

int Window::iterm2_ = -1;

}  // namespace dcurses
