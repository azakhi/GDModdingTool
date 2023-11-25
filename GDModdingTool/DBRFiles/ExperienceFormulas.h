#pragma once

#include "DBRBase.h"

class ExperienceFormulas : public DBRBase {
    bool _isParsed = false;

public:
    ExperienceFormulas(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName, bool isAlwaysDirty = false)
        : DBRBase(fileManager, directoryEntry, templateName, isAlwaysDirty) {}
    void parse();
    void adjustExpGained(DifficultyType difficulty, float multiplier);
    void setExpGainedEquation(DifficultyType difficulty, std::string value);
};
