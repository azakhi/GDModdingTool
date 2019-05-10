#pragma once

#include "DBRBase.h"
#include "Lootable.h"

class FixedItemLoot : public DBRBase
{
    bool _isParsed = false;
    std::vector<Lootable> _lootables;
    float _modifiedLootAmount = 1.0f;

    void _updateLootEquations();
public:
    FixedItemLoot(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName)
        : DBRBase(fileManager, directoryEntry, templateName) {}
    void parse();
    void updateFromChild();
    void adjustLootAmount(float multiplier);
    void adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types = std::vector<ItemType>(), std::vector<ItemClass> rarities = std::vector<ItemClass>(), bool isAnd = false);

    const bool isDirty() const {
        for (const auto& l : _lootables) {
            if (l.isDirty()) {
                return true;
            }
        }

        return DBRBase::isDirty();
    }
};