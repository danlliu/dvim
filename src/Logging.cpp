// Copyright 2022 Daniel Liu

// Logging functionality

#include "Logging.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <string>

Logger::Logger() : file_(LOGFILE) {}

void Logger::log(const std::string& message) {
  auto time = std::chrono::system_clock::now();
  auto timestamp = std::chrono::system_clock::to_time_t(time);
  file_ << std::ctime(&timestamp) << " " << message << "\n";
  file_.flush();
}
