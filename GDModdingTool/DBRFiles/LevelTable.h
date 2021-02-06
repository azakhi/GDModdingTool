#pragma once

#include "DBRBase.h"
#include "DynWeightAffixTable.h"

class LevelTable : public DBRBase
{
    bool _isParsed = false;
    std::vector<DynWeightAffixTable*> _records;
    std::vector<LevelTable*> _levelTables;

public:
    LevelTable(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName, bool isAlwaysDirty = false)
        : DBRBase(fileManager, directoryEntry, templateName, isAlwaysDirty) {}
    void parse() {
        if (_isParsed) return;
        DBRBase::parse();
        Field* recordsField = _fieldMap["records"];
        if (recordsField != nullptr) {
            std::stringstream ss(recordsField->value());
            std::string item = "";
            while (std::getline(ss, item, ';')) {
                DBRBase* file = _fileManager->getFile(item);
                if (file == nullptr) {
                    log_error << "Loot file is not found\n    Level Table: " << _directoryEntry.path().string() << "\n    File: " << item << "\n";
                }
                else if (file->templateName() == "leveltable.tpl") {
                    _levelTables.push_back((LevelTable*)file);
                }
                else if (dynamic_cast<DynWeightAffixTable*>(file))
                {
                    _records.push_back((DynWeightAffixTable*)file);
                }
                else {
                    log_error << "Unknown file type in Level Table\n    Level Table: " << _directoryEntry.path().string() << "\n    Template: " << file->templateName() << "\n";
                }
            }
        }
    }

    void records(std::vector<DynWeightAffixTable*>& records) {
        records.insert(records.end(), _records.begin(), _records.end());
        for (auto l : _levelTables) {
            l->records(records);
        }
    }
};