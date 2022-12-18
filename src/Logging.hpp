// Copyright 2022 Daniel Liu

// Logging functionality

#include <fstream>
#include <string>

#ifdef LOG_ON
#define LOGFILE "dvim.log"
#define LOG(message) Logger::logger().log(message)
#else
#define LOGFILE ""
#define LOG(message)
#endif

class Logger {
 public:
  static Logger& logger() { static Logger logger; return logger; };

  void log(const std::string& message);
 private:
  Logger();
  Logger(const Logger&) = delete;
  void operator=(const Logger&) = delete;
  Logger(const Logger&&) = delete;
  void operator=(const Logger&&) = delete;

  std::ofstream file_;
};
