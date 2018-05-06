#ifndef DISTRIBUTEDMONITOR_LOGGER_H
#define DISTRIBUTEDMONITOR_LOGGER_H


#include <iomanip>
#include <mutex>
#include <iostream>
#include "Singleton.h"

enum LogLevel {
    NONE = 0,
    NORMAL = 1,
    DEBUG = 2,
};

class Logger {
private:
    std::mutex mutex;
    LogLevel logLevel = LogLevel::NORMAL;

public:
    Logger();

    void log(LogLevel logLevel, const std::string message);

    void setLogLevel(LogLevel logLevel);
};

typedef Singleton<Logger> LoggerSingleton;


#endif //DISTRIBUTEDMONITOR_LOGGER_H
