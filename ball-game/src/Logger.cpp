#include "../include/Logger.h"
#include "../include/Config.h"
#include <iostream>
#include <sys/stat.h>

// Static member initialization
std::string Logger::m_logFilePath = "";
bool Logger::m_initialized = false;

void Logger::initialize(const std::string& logDir) {
    // Create log directory if it doesn't exist
    mkdir(logDir.c_str(), 0755);
    // Include PID in log filename
    pid_t pid = getpid();
    m_logFilePath = logDir + "/ball_game_" + std::to_string(pid) + ".log";
    m_initialized = true;
}

std::string Logger::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

std::string Logger::getLogFileName() {
    return m_logFilePath;
}

void Logger::log(LogLevel level, const std::string& file, const std::string& procedure,
                const std::string& message) {
    if (!m_initialized) {
        initialize();
    }

    std::string dateTime = getCurrentDateTime();
    std::string levelStr = getLevelString(level);

    // Format: DATE - TIME - LEVEL - FILE - PROCEDURE - MESSAGE
    std::ostringstream logStream;
    logStream << dateTime << " - " << levelStr << " - " << file << " - " << procedure << " - " << message;
    std::string logMessage = logStream.str();

    // Write to file
    std::ofstream logFile(m_logFilePath, std::ios::app);
    if (logFile.is_open()) {
        logFile << logMessage << std::endl;
        logFile.close();
    } else {
        std::cerr << "Failed to open log file: " << m_logFilePath << std::endl;
    }

    // Also print to console for debugging
    std::cout << logMessage << std::endl;
}

bool Logger::printLogLevel(LogLevel level) {
    return static_cast<int>(level) >= static_cast<int>(LOG_LEVEL);
}

void Logger::debug(const std::string& file, const std::string& procedure, const std::string& message) {
    if (printLogLevel(LogLevel::DEBUG)) {
        log(LogLevel::DEBUG, file, procedure, message);
    }
}

void Logger::info(const std::string& file, const std::string& procedure, const std::string& message) {
    if (printLogLevel(LogLevel::INFO)) {
        log(LogLevel::INFO, file, procedure, message);
    }
}

void Logger::warning(const std::string& file, const std::string& procedure, const std::string& message) {
    if (printLogLevel(LogLevel::WARNING)) {
        log(LogLevel::WARNING, file, procedure, message);
    }
}

void Logger::error(const std::string& file, const std::string& procedure, const std::string& message) {
    if (printLogLevel(LogLevel::ERROR)) {
        log(LogLevel::ERROR, file, procedure, message);
    }
}

void Logger::critical(const std::string& file, const std::string& procedure, const std::string& message) {
    if (printLogLevel(LogLevel::CRITICAL)) {
        log(LogLevel::CRITICAL, file, procedure, message);
    }
}
