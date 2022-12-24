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
#include "Logging.hpp"
#include "dvim/Utilities.hpp"

#define ESC "\33"

namespace dcurses {

Window::Window(
  unsigned int row, unsigned int col, unsigned int width, unsigned int height, 
  int zIndex, const WindowBorder &border) :
  row_ {row}, col_ {col}, width_ {width}, height_ {height}, zIndex_ {zIndex}, 
  border_ {border}, cache_ {height, {width, " "}} {
  clear();
}

void Window::setCharacter(unsigned int row, unsigned int col, char character) {
  directions_.emplace_back(RenderDirection{ row, col, character });
}

void Window::setString(unsigned int row, unsigned int col, const std::string& string) {
  directions_.emplace_back(RenderDirection{ row, col, string });
}

void Window::drawImage(const ImageContent &image) {
  if (iterm2_ == 1) {
    directions_.emplace_back(RenderDirection{ image.row_, image.col_, image });
    hasImages_ = true;
  } else {
    directions_.emplace_back(RenderDirection{ image.row_, image.col_, std::string{ "Image content." }});
  }
}

void Window::refresh() {
  if (hasImages_) {
    // purge the window and cache
    std::cout << ESC << "[0m";
    for (unsigned int r = 0; r < height_; ++r) {
      unsigned int line = row_ + r + 1;
      unsigned int col = col_ + 1;
      std::cout << ESC << "[" << line << ";" << col << "f";
      std::cout << std::string(width_, ' ');
      for (unsigned int c = 0; c < width_; ++c) {
        cache_[r][c] = " ";
      }
    }

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
        if (getTmux() == 1) {
          std::cout << "\033Ptmux;\033\033]";
        } else {
          std::cout << "\033]";
        }
        std::cout << "1337;File=inline=1;size=" << size(image.content_);
        std::cout << ";width=" << image.width_ << ";height=" << image.height_;
        std::cout << ":" << dcurses::base64Encode(image.content_);
        if (getTmux() == 1) {
          std::cout << "\a\033\\";
        } else {
          std::cout << "\a";
        }
      }
    }
    std::cout << std::flush;
  } else {
    // text only, use cache
    std::vector<std::vector<std::string>> newContent {height_, {width_, " "}};
    for (const auto &direction : directions_) {
      unsigned int row = direction.row;
      unsigned int col = direction.col;
      if (std::holds_alternative<char>(direction.content)) {
        const auto &character = std::get<char>(direction.content);
        newContent[row][col] = std::string{character};
      } else if (std::holds_alternative<std::string>(direction.content)) {
        const auto &string = std::get<std::string>(direction.content);
        for (auto character : dvim::splitVisibleCharacters(string)) {
          newContent[row][col++] = character;
        }
      }
    }

    for (unsigned int r = 0; r < height_; ++r) {
      for (unsigned int c = 0; c < width_; ++c) {
        if (newContent[r][c] != cache_[r][c]) {
          unsigned int line = row_ + r + 1;
          unsigned int col = col_ + c + 1;
          std::cout << ESC << "[" << line << ";" << col << "f";
          std::cout << newContent[r][c];
          cache_[r][c] = newContent[r][c];
        }
      }
    }
    std::cout << std::flush;
  }
}

void Window::clear() {
  directions_.clear();
  if (hasImages_) {
    // purge the window and cache
    std::cout << ESC << "[0m";
    for (unsigned int r = 0; r < height_; ++r) {
      unsigned int line = row_ + r + 1;
      unsigned int col = col_ + 1;
      std::cout << ESC << "[" << line << ";" << col << "f";
      std::cout << std::string(width_, ' ');
      for (unsigned int c = 0; c < width_; ++c) {
        cache_[r][c] = "";
      }
    }
  }
  hasImages_ = false;

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

void Window::clearCache() {
  std::cout << ESC << "[0m";
  for (unsigned int r = 0; r < height_; ++r) {
    unsigned int line = row_ + r + 1;
    unsigned int col = col_ + 1;
    std::cout << ESC << "[" << line << ";" << col << "H";
    std::cout << std::string(width_, ' ');
  }
  cache_ = std::vector<std::vector<std::string>>(height_, {width_, ""});
}

int Window::iterm2_ = -1;
int Window::tmux_ = -1;

}  // namespace dcurses
