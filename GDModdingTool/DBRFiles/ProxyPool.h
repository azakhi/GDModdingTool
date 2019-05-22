#pragma once

#include "DBRBase.h"
#include "Field.h"

class ProxyPool : public DBRBase
{
    bool _isParsed = false;
    bool _ignoreGameBalance = false;
    NumericField<float>* _championChance;
    NumericField<int>* _championMax;
    NumericField<int>* _championMin;
    NumericField<int>* _spawnMax;
    NumericField<int>* _spawnMin;
    std::vector<Spawn> _spawns[2] = { std::vector<Spawn>(), std::vector<Spawn>() };

    bool _isRatioPreserved() const;
public:
    ProxyPool(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName)
        : DBRBase(fileManager, directoryEntry, templateName) {}
    void parse();
    void adjustSpawnAmount(float multiplier);
    void adjustSpawnMin(float multiplier);
    void adjustSpawnMax(float multiplier);
    void adjustChampionSpawnAmount(float multiplier);
    void adjustChampionSpawnMin(float multiplier);
    void adjustChampionSpawnMax(float multiplier);
    void adjustChampionChance(float multiplier);
    void adjustMonsterClassWeight(MonsterClass monsterClass, float multiplier);
    void increaseMonsterClassLimit(MonsterClass monsterClass, int limit);
    void setMonsterClassMaxPlayerLevel(MonsterClass monsterClass, int level);
    void setMonsterClassMinPlayerLevel(MonsterClass monsterClass, int level);

    const bool isIgnoreGameBalance() const {
        return _ignoreGameBalance;
    }

    const bool isDirty() const {
        return !_ignoreGameBalance && (DBRBase::isDirty() || !_isRatioPreserved());
    }
};