#include "Logger.h"
#include <ctime>
#include <iostream>

Logger::~Logger() {
    if (_logFile.is_open())
        _logFile.close();

    delete _logger;
}

Log* Logger::logger() {
    static Logger instance;
    return instance.init();
}

Log* Logger::init() {
    if (_logFile.is_open()) {
        return _logger;
    }

    std::string logFileName = "Logs_";
    std::time_t t = time(0);
    struct tm now;
    localtime_s(&now, &t);
    char buffer[20];
    strftime(buffer, 20, "%d.%m.%y_%H.%M.%S", &now);
    logFileName += buffer;
    logFileName += ".txt";
    _logFile.open(logFileName, std::ios::out | std::ios::trunc);
    if (!_logFile) {
        std::cout << "Couldn't open log file\n";
        return nullptr;
    }

    delete _logger;
    _logger = new Log();
    _logger->logFile = &_logFile;
    return _logger;
}