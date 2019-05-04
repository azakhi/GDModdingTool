#pragma once

#include <functional>
#include <mutex>
#include <typeindex>

#include "FileManager.h"
#include "Templates/Template.h"
#include "Templates/FixedItemLoot.h"
#include "Templates/LootMasterTable.h"
#include "Templates/Monster.h"
#include "Templates/ProxyPool.h"
#include "Templates/ItemBase.h"
#include "Templates/DynWeightAffixTable.h"
#include "Templates/LevelTable.h"

class Customizer
{
    const int THREAD_COUNT = 16;
    FileManager* _fileManager;
    std::vector<std::function<void()>> _tasks;
    std::vector<std::type_index> _preParseFiles;
    std::vector<std::string> _preParseFilesStr;

    std::mutex _workPartMutex;
    int _remainingWorkStart;
    std::vector<int> _threadProgress;
    std::vector<bool> _isThreadDone;
    int _progressTotal;

public:
    Customizer(FileManager* fileManager);

    const bool isWorkersDone() const {
        for (const auto itd : _isThreadDone) {
            if (!itd) return false;
        }

        return true;
    }

    const int progressTotal() const { return _progressTotal; }

    const std::string progress() const {
        int total = 0;
        for (const auto tp : _threadProgress) total += tp;
        std::ostringstream os;
        os << std::fixed << std::setprecision(1) << "Progress: " << (100 * total / (float)(_progressTotal)) << "% (" << total << "/" << _progressTotal << ")\n";
        return os.str();
    }

    void preParse();
    void runTasks();
    void adjustLifeIncrement(float multiplier);
    void setMaxDevotionPoints(int point);
    void setMaxLevel(int level);
    void adjustGoldDrop(float multiplier);
    void adjustExpRequirement(float multiplier);
    void setDevotionPointsPerShrine(int point);
    void adjustCommonSpawnAmount(float multiplier);
    void adjustChampionSpawnAmount(float multiplier);
    void adjustChampionChance(float multiplier);
    void adjustSpawnAmount(float multiplier);
    void adjustLootAmount(float multiplier, LootSource source = LootSource::All);
    void adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types = std::vector<ItemType>(), std::vector<ItemClass> rarities = std::vector<ItemClass>(), bool isAnd = false);
    void removeDifficultyLimits();
    void adjustMonsterClassWeight(MonsterClass monsterClass, float multiplier);
    void increaseMonsterClassLimit(MonsterClass monsterClass, int limit);
    void setMonsterClassMaxPlayerLevel(MonsterClass monsterClass, int level);
    void setMonsterClassMinPlayerLevel(MonsterClass monsterClass, int level);
    void adjustFactionRepRequirements(float multiplier);
    void setItemStackLimit(ItemType type, int limit);

private:
    template <typename T>
    void _addFileForPreParse();
    void _addFileForPreParse(std::string s);
    void _preParse(int tnum, std::vector<Template*> temps);
    void _preParseWOThreads();
    void _getWorkPart(int& start, int& end);
};