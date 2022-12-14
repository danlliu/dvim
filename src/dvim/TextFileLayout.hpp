// Copyright 2022 Daniel Liu

// Function to lay out the contents of a text file on a TUI.

#include <string>
#include <vector>

namespace dvim {

/*
 * Display the given file contents, with the specified viewport width.
 */
std::vector<std::string> layoutFile(const std::string& fileContents, unsigned int width);

/*
 * Display the given file contents, with the specified viewport width and line numbers.
 */
std::vector<std::string> layoutFileWithLineNums(const std::string& fileContents, unsigned int width);

}
