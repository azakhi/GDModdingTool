#pragma once

#include "Template.h"
#include "Lootable.h"

class LootMasterTable : public Template
{
    bool _isParsed = false;

protected:
    Lootable _lootable;

public:
    LootMasterTable(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName)
        : Template(fileManager, directoryEntry, templateName) {}
    void parse();
    void adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types = std::vector<ItemType>(), std::vector<ItemClass> rarities = std::vector<ItemClass>(), bool isAnd = false);

    int totalWeight() const {
        return _lootable.totalWeight();
    }

    int totalModifiedWeight() const {
        return _lootable.totalModifiedWeight();
    }

    const bool isDirty() const {
        return _lootable.isDirty() || Template::isDirty();
    }

private:

};