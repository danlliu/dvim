// Copyright 2022 Daniel Liu

// File system list implementation.

#ifndef DVIM_FILE_TREE_HPP_
#define DVIM_FILE_TREE_HPP_

#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace dvim {

/*
 * Generates the icon for a specified extension. If ASCIIONLY is defined, 
 * defaults to "*".
 */
std::string ext2icon(const std::string &ext);

class FileTree {
 public:
  /*
   * Constructs the file tree object, which represents the given path and all of
   * its subpaths.
   */
  explicit FileTree(const std::filesystem::path &path);

  /*
   * Opens the specified path, allowing subpaths to be viewed.
   */
  void open(std::filesystem::path path);

  /*
   * Closes the specified path, stopping subpaths from being viewed.
   */
  void close(std::filesystem::path path);

  /*
   * Toggles the visibility of the specified path.
   */
  void toggle(std::filesystem::path path);

  /*
   * Generates a list of lines, each corresponding to a file or directory in the
   * current path.
   */
  std::vector<std::string> toString();

  /*
   * Retrieves the path corresponding to the specified line in the most recent
   * output of toString.
   */
  std::filesystem::path lineToPath(unsigned int line) const {
    if (line < 0 || line >= size(line2path_)) return ".";
    return line2path_[line]; 
  }

 private:
  std::vector<std::pair<std::string, std::filesystem::path>> stringifyPath(const std::filesystem::path &path) const;

  std::vector<std::filesystem::path> line2path_;

  std::filesystem::path path_;
  std::set<std::filesystem::path> openPaths_;
};

}  // namespace dvim

#endif
