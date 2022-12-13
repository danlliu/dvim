// Copyright 2022 Daniel Liu

// File system list implementation.

#ifndef DVIM_FILE_TREE_HPP_
#define DVIM_FILE_TREE_HPP_

#include <filesystem>
#include <set>
#include <string>
#include <vector>

namespace dvim {

std::string ext2icon(const std::string &ext);

class FileTree {
 public:
  explicit FileTree(const std::filesystem::path &path);

  void open(std::filesystem::path path);

  void close(std::filesystem::path path);

  std::vector<std::string> toString();

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
