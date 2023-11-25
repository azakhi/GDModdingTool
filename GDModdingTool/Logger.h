#pragma once
#include <fstream>

struct Log {
    std::ofstream* logFile = NULL;
    std::string logFileName = "";

    template <typename T>
    Log& operator<<(T const& obj) {
        if (logFile != NULL)
            (*logFile) << obj;

        return *this;
    }

    Log& operator<<(std::ostream& (*pf)(std::ostream&)) {
        if (logFile != NULL)
            (*logFile) << pf;

        return *this;
    }
};

class Logger {
private:
    std::ofstream _logFile;
    Logger() {}
    Log* _logger = nullptr;
public:
    ~Logger();
    Logger(Logger& copy) {}
    Log* init();
    static std::string LogFileName() { return Logger::logger()->logFileName; };
    static Log* logger();
    static int ErrorCount;
    static Log* error() {
        auto logger = Logger::logger();
        (*logger) << "ERROR: ";
        ErrorCount++;
        return logger;
    }
};