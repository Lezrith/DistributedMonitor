#ifndef DISTRIBUTEDMONITOR_LOGGER_H
#define DISTRIBUTEDMONITOR_LOGGER_H


#include <iomanip>
#include <mutex>
#include <iostream>
#include "Singleton.h"

class Logger {
private:
    std::mutex mutex;

public:
    Logger();

    void log(std::string message);
};

typedef Singleton<Logger> LoggerSingleton;


#endif //DISTRIBUTEDMONITOR_LOGGER_H
