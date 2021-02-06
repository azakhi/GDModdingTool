#pragma once

#include <unordered_map>

#include "DBRBase.h"
#include "Lootable.h"

class Monster : public DBRBase
{
    bool _isParsed = false;
    std::unordered_map<std::string, Lootable> _lootables;
    float _modifiedLootAmount = 1.0f;
    MonsterClass _monsterClass;
    int _faction;

public:
    Monster(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName, bool isAlwaysDirty = false)
        : DBRBase(fileManager, directoryEntry, templateName, isAlwaysDirty) {}
    void parse();
    void updateFromChild();
    void adjustLootAmount(float multiplier);
    void adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types = std::vector<ItemType>(), std::vector<ItemClass> rarities = std::vector<ItemClass>(), bool isAnd = false);

    const MonsterClass monsterClass() {
        parse();
        return _monsterClass;
    }

    const int faction() {
        parse();
        return _faction;
    }

    const bool isDirty() const {
        if (_isAlwaysDirty) {
            return true;
        }

        for (const auto&[key, l] : _lootables) {
            if (l.isDirty()) {
                return true;
            }
        }

        return DBRBase::isDirty();
    }

private:
};