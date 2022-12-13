// Copyright 2022 Daniel Liu

// Usage hint panel layout.

#ifndef DVIM_USAGE_HINT_HPP_
#define DVIM_USAGE_HINT_HPP_

#include <algorithm>
#include <string>
#include <vector>

namespace dvim {

std::vector<std::string> layoutUsageHints(const std::vector<std::string> &hints, unsigned int width);

}

#endif
