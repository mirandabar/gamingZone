#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <unistd.h>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
private:
    static std::string m_logFilePath;
    static bool m_initialized;

public:
    // Initialize the logger with log directory
    static void initialize(const std::string& logDir = "log");

    // Log a message
    static void log(LogLevel level, const std::string& file, const std::string& procedure,
                    const std::string& message);

    // Convenience methods
    static void debug(const std::string& file, const std::string& procedure, const std::string& message);
    static void info(const std::string& file, const std::string& procedure, const std::string& message);
    static void warning(const std::string& file, const std::string& procedure, const std::string& message);
    static void error(const std::string& file, const std::string& procedure, const std::string& message);
    static void critical(const std::string& file, const std::string& procedure, const std::string& message);

private:
    // Helper methods
    static std::string getLevelString(LogLevel level);
    static std::string getCurrentDateTime();
    static std::string getLogFileName();
};
