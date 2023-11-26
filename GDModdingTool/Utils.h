#pragma once
#include <string>

#include "Logger.h"

#ifndef log_info
#define log_info (*Logger::logger())
#define log_warning (*Logger::logger())<<"WARNING: "
#define log_debug (*Logger::logger())<<"DEBUG: "
#define log_error (*Logger::error())
#endif // !log_info

enum ThreadStatus { NotRunning, Running, Completed, ThrownError };

inline std::string StringTrim(const std::string& str, const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

struct Config
{
    std::string recordsDir = "";
    std::string addRecordsDir = "";
    std::string modDir = "";
    std::vector<std::string> includedModDirs;
    std::vector<std::string> subDirs;
    std::vector<std::string> addSubDirs;
    std::vector<std::string> commands;
    bool isAddInventoryBagsAtStart = false;
    bool isAddCaravanExtreme = false;
};