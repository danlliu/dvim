// Copyright 2022 Daniel Liu

// Usage hint panel layout.

#include "UsageHint.hpp"

#include <algorithm>
#include <string>
#include <vector>

namespace dvim {

std::vector<std::string> layoutUsageHints(const std::vector<std::string> &hints, unsigned int width) {
  if (size(hints) == 0) {
    return {};
  }

  unsigned long longestHint = std::max_element(hints.begin(), hints.end(), [](const std::string &a, const std::string &b) {
    return a.size() < b.size();
  })->size();
  unsigned long columns = width / longestHint;
  if (columns > size(hints)) {
    columns = size(hints);
  }
  unsigned long spacingLeft = width - columns * longestHint;
  std::vector<std::string> result;
  for (unsigned int i = 0; i < size(hints); i += columns) {
    std::string line = "";
    for (unsigned int j = 0; j < columns; j++) {
      line += hints[i + j];
      line += std::string(longestHint - hints[i + j].size(), ' ');
      line += std::string((spacingLeft + j) / columns, ' ');
    }
    result.emplace_back(line);
  }
  return result;
}

}
