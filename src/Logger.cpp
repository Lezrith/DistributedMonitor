#include "Logger.h"

Logger::Logger() = default;

void Logger::log(const std::string message) {
    std::lock_guard<std::mutex> guard(mutex);
    time_t rawtime;

    char timeString[80];

    time(&rawtime);
    std::tm *timeInfo = localtime(&rawtime);

    strftime(timeString, sizeof(timeString), "[%H:%M:%S]", timeInfo);
    std::cout << timeString << " " << message << std::endl;
}
