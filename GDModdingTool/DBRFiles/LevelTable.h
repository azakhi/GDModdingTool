#pragma once

#include "DBRBase.h"
#include "DynWeightAffixTable.h"

class LevelTable : public DBRBase
{
    bool _isParsed = false;
    std::vector<DynWeightAffixTable*> _records;

public:
    LevelTable(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName)
        : DBRBase(fileManager, directoryEntry, templateName) {}
    void parse() {
        if (_isParsed) return;
        DBRBase::parse();
        Field* recordsField = _fieldMap["records"];
        if (recordsField != nullptr) {
            std::stringstream ss(recordsField->value());
            std::string item = "";
            while (std::getline(ss, item, ';')) {
                DBRBase* file = _fileManager->getFile(item);
                if (dynamic_cast<DynWeightAffixTable*>(file))
                {
                    _records.push_back((DynWeightAffixTable*)file);
                }
                else {
                    throw std::runtime_error("Unknown file type in Level Table");
                }
            }
        }
    }

    std::vector<DynWeightAffixTable*> records() {
        return _records;
    }
};