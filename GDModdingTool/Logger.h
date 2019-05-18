#pragma once
#include <fstream>

struct Log {
    std::ofstream* logFile = NULL;

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
    static Log* logger();
};