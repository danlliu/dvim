// Copyright 2022 Daniel Liu

// File system list implementation

#include "FileTree.hpp"

#include <filesystem>
#include <set>
#include <string>
#include <vector>

#ifdef ASCIIONLY
#define CLOSEDFOLDER std::string{"d"}
#define OPENFOLDER std::string{"+"}
#define LEADER std::string{"|"}
#define LEADERBOTTOM std::string{"+"}
#else
#define CLOSEDFOLDER std::string{"\ue5ff"}
#define OPENFOLDER std::string{"\ue5fe"}
#define LEADER std::string{"\u2502"}
#define LEADERBOTTOM std::string{"\u2514"}
#endif

namespace dvim {

std::string ext2icon(const std::filesystem::path &path) {
#ifdef ASCIIONLY
  return "*";
#else
  // Special file names
  if (path.filename() == ".gitignore") return "\33[38;5;196m\uf7a1\33[0m";
  if (path.filename() == "Makefile") return "\ufd1c";

  std::string ext = path.extension().string();
  if (ext == ".c") return "\33[38;5;26m\ufb70\33[0m";
  if (ext == ".cpp") return "\33[38;5;26m\ufb71\33[0m";
  if (ext == ".h") return "\33[38;5;26mh\33[0m";
  if (ext == ".hpp") return "\33[38;5;26mh\33[0m";
  if (ext == ".o") return "\uf471";
  if (ext == ".html") return "\33[38;5;166m\uf81c\33[0m";
  if (ext == ".css") return "\33[38;5;21m\uf81b\33[0m";
  if (ext == ".py") return "\33[38;5;226m\uf81f\33[0m";
  if (ext == ".png" || ext == ".jpg") return "\uf71e";
  return "\uf713";
#endif
}

FileTree::FileTree(const std::filesystem::path &path) : path_(path) {}

void FileTree::open(std::filesystem::path path) {
  openPaths_.insert(path);
}

void FileTree::close(std::filesystem::path path) {
  openPaths_.erase(path);
}

void FileTree::toggle(std::filesystem::path path) {
  if (openPaths_.count(path)) {
    close(path);
  } else {
    open(path);
  }
}

std::vector<std::string> FileTree::toString() {
  auto res = stringifyPath(path_);
  line2path_ = {};
  std::vector<std::string> result;
  for (auto &p : res) {
    result.emplace_back(p.first);
    line2path_.push_back(p.second);
  }
  return result;
}

std::vector<std::pair<std::string, std::filesystem::path>> FileTree::stringifyPath(const std::filesystem::path &path) const {
  if (!openPaths_.count(path)) {
    return {{CLOSEDFOLDER + " " + path.filename().string(), path}};
  }
  std::vector<std::pair<std::string, std::filesystem::path>> result 
    = {{OPENFOLDER + " " + path.filename().string(), path}};
  
  int i = 0;
  for ([[maybe_unused]] const auto &entry : std::filesystem::directory_iterator(path)) {
    ++i;
  }
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    --i;
    auto leader = (i == 0 ? LEADERBOTTOM : LEADER);
    if (entry.is_directory()) {
      auto sub = stringifyPath(entry.path());
      for (unsigned int j = 0; j < size(sub); ++j) {
        auto l = sub[j];
        if (j == size(sub) - 1) {
          result.emplace_back(std::make_pair(leader + " " + l.first, l.second));
        } else {
          result.emplace_back(std::make_pair(LEADER + " " + l.first, l.second));
        }
      }
    } else {
      std::string entryStr = leader + " ";
      entryStr += ext2icon(entry.path());
      entryStr += " ";
      entryStr += entry.path().filename().string();
      result.emplace_back(std::make_pair(entryStr, entry.path()));
    }
  }
  return result;
}

}  // namespace dvim