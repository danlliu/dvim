// Copyright 2022 Daniel Liu

// Usage hint panel layout.

#ifndef DVIM_USAGE_HINT_HPP_
#define DVIM_USAGE_HINT_HPP_

#include <algorithm>
#include <string>
#include <vector>

namespace dvim {

/*
 * Layout manager for usage hints. Takes a list of usage hints and a width, and
 * returns a vector of lines. The hints are spread across the width equally,
 * and fill in from left to right, top to bottom.
 */
std::vector<std::string> layoutUsageHints(const std::vector<std::string> &hints, unsigned int width);

}

#endif
