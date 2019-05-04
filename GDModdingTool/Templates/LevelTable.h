#pragma once

#include "Template.h"
#include "DynWeightAffixTable.h"

class LevelTable : public Template
{
    bool _isParsed = false;
    std::vector<DynWeightAffixTable*> _records;

public:
    LevelTable(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName)
        : Template(fileManager, directoryEntry, templateName) {}
    void parse() {
        if (_isParsed) return;
        Template::parse();
        auto it = _fields.find("records");
        if (it != _fields.end()) {
            std::stringstream ss(it->second.second->value());
            std::string item = "";
            while (std::getline(ss, item, ';')) {
                Template* file = _fileManager->getFile(item);
                if (dynamic_cast<DynWeightAffixTable*>(file))
                {
                    _records.push_back((DynWeightAffixTable*)file);
                }
                else {
                    throw "Unknown file type in Level Table";
                }
            }
        }
    }

    std::vector<DynWeightAffixTable*> records() {
        return _records;
    }
};