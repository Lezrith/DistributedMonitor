#include "Logger.h"

Logger::Logger() = default;

void Logger::log(LogLevel logLevel, const std::string message) {
    if (this->logLevel >= logLevel) {
        std::lock_guard<std::mutex> guard(this->mutex);
        time_t rawTime;

        char timeString[80];

        time(&rawTime);
        std::tm *timeInfo = localtime(&rawTime);

        strftime(timeString, sizeof(timeString), "[%H:%M:%S]", timeInfo);
        std::cout << timeString << " " << message << std::endl;
    }
}

void Logger::setLogLevel(LogLevel logLevel) {
    std::lock_guard<std::mutex> guard(this->mutex);
    Logger::logLevel = logLevel;
}
