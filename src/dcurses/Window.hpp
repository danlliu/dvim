// Copyright 2022 Daniel Liu

// Window.hpp
// C++ API for ncurses-like window management.

#ifndef DCURSES_WINDOW_HPP_
#define DCURSES_WINDOW_HPP_

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#ifdef ASCIIONLY
#define DEFAULT_BORDER {"+","-","+","|","+","-","+","|"}
#else
#define DEFAULT_BORDER {"\u250c","\u2500","\u2510","\u2502","\u2518","\u2500","\u2514","\u2502"}
#endif

namespace dcurses {

class Window {
 public:
  struct WindowBorder {
    std::string top_left_;
    std::string top_;
    std::string top_right_;
    std::string right_;
    std::string bottom_right_;
    std::string bottom_;
    std::string bottom_left_;
    std::string left_;
  };

  struct ImageContent {
    unsigned int row_;
    unsigned int col_;
    unsigned int width_;
    unsigned int height_;
    std::string content_;
  };

  /*
   * Construct a new Window object with the specified width, height, and border characters.
   * @param width The width of the window, in characters.
   * @param height The height of the window, in characters.
   * @param border The border characters, in the following order: top-left, top, top-right, right, bottom-right, bottom, bottom-left, left.
   */
  Window(
    unsigned int row, unsigned int col, unsigned int width, unsigned int height, 
    int zIndex, const WindowBorder &border = DEFAULT_BORDER);

  /*
   * Sets the character at the specified postion to the specified character.
   * Characters are indexed from 0, with (0,0) being the top-left border character.
   * @param row The row of the character to set.
   * @param col The column of the character to set.
   * @param character The character to set.
   */
  void setCharacter(unsigned int row, unsigned int col, char character);

  /*
   * Displays the specified string at the specified position.
   * Characters are indexed from 0, with (0,0) being the top-left border character.
   * @param row The row of the top left corner of the string.
   * @param col The column of the top left corner of the string.
   * @param character The string to set.
   */
  void setString(unsigned int row, unsigned int col, const std::string& string);

  /*
   * Renders an image at the specified position. The contents and sizing of the image are specified by the ImageContent struct passed in.
   * Characters are indexed from 0, with (0,0) being the top-left border character.
   * @param row The row of the top left corner of the image.
   * @param col The column of the top left corner of the image.
   */
  void drawImage(unsigned int row, unsigned int col, const ImageContent &image);

  /*
   * Renders the window onto the screen, using `row_` and `col_` as the top left corner.
   */
  void refresh();

  /*
   * Clears the window, removing all contents.
   */
  void clear();

  /*
   * Get the row of the window's top left corner.
   */
  unsigned int row() const { return row_; }

  /*
   * Get the column of the window's top left corner.
   */
  unsigned int col() const { return col_; }

  /*
   * Get the width of the window, in characters.
   */
  unsigned int width() const { return width_; }

  /*
   * Get the height of the window, in characters.
   */
  unsigned int height() const { return height_; }

  /*
   * Get the z-index of the window.
   */
  int zIndex() const { return zIndex_; }

 private:
  struct RenderDirection {
    unsigned int row;
    unsigned int col;
    std::variant<char, std::string, ImageContent> content;
  };
  std::vector<RenderDirection> directions_;

  unsigned int row_;
  unsigned int col_;
  unsigned int width_;
  unsigned int height_;
  int zIndex_;
  WindowBorder border_;
};

}  // namespace dcurses

#endif
