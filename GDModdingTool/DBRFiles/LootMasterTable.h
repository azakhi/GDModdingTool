#pragma once

#include "DBRBase.h"
#include "Lootable.h"

class LootMasterTable : public DBRBase
{
    bool _isParsed = false;

protected:
    Lootable _lootable;

public:
    LootMasterTable(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName, bool isAlwaysDirty = false)
        : DBRBase(fileManager, directoryEntry, templateName, isAlwaysDirty) {}
    void parse();
    void adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types = std::vector<ItemType>(), std::vector<ItemClass> rarities = std::vector<ItemClass>(), bool isAnd = false);

    int totalWeight() const {
        return _lootable.totalWeight();
    }

    int totalModifiedWeight() const {
        return _lootable.totalModifiedWeight();
    }

    const bool isDirty() const {
        return _isAlwaysDirty || _lootable.isDirty() || DBRBase::isDirty();
    }

private:

};