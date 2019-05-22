#pragma once

#include <functional>
#include <mutex>
#include <typeindex>
#include <unordered_map>

#include "FileManager.h"
#include "DBRFiles/DBRBase.h"
#include "DBRFiles/FixedItemLoot.h"
#include "DBRFiles/LootMasterTable.h"
#include "DBRFiles/Monster.h"
#include "DBRFiles/ProxyPool.h"
#include "DBRFiles/ItemBase.h"
#include "DBRFiles/DynWeightAffixTable.h"
#include "DBRFiles/LevelTable.h"
#include "DBRFiles/ExperienceLevelControl.h"

class ParamTypes
{
public:
    // Types
    static const std::function<int(std::string)> Integer;
    static const std::function<float(std::string)> Float;
    static const std::function<bool(std::string)> Boolean;
    static const std::function<AffixType(std::string)> AffixTypeEnum;
    static const std::function<LootSource(std::string)> LootSourceEnum;
    static const std::function<MonsterClass(std::string)> MonsterClassEnum;
    static const std::function<ItemType(std::string)> ItemTypeEnum;
    static const std::function<ItemClass(std::string)> ItemClassEnum;
    template <typename T>
    static const std::function<std::vector<T>(std::string, std::function<T(std::string)>)> Vector;

    // Validators
    static const std::function<bool(std::string)> IntegerValidator;
    static const std::function<bool(std::string)> FloatValidator;
    static const std::function<bool(std::string)> BooleanValidator;
    static const std::function<bool(std::string)> AffixTypeEnumValidator;
    static const std::function<bool(std::string)> LootSourceEnumValidator;
    static const std::function<bool(std::string)> MonsterClassEnumValidator;
    static const std::function<bool(std::string)> ItemTypeEnumValidator;
    static const std::function<bool(std::string)> ItemClassEnumValidator;
    static const std::function<bool(std::string, std::function<bool(std::string)>)> VectorValidator;
};

class Customizer
{
    const int THREAD_COUNT = 16;

    class Command
    {
    public:
        Command() {};
        Command(std::vector<std::function<bool(std::string)>> vs, std::function<void(std::vector<std::string>)> m) : validators(vs), method(m) {}
        std::vector<std::function<bool(std::string)>> validators;
        std::function<void(std::vector<std::string>)> method;
        bool isValid(std::vector<std::string> params) const {
            if (params.size() < validators.size()) return false;
            for (size_t i = 0; i < validators.size(); i++) {
                if (!validators[i](params[i])) {
                    return false;
                }
            }

            return true;
        }
    };

    std::unordered_map<std::string, Command> _commandMap;
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
    Customizer(FileManager* fileManager, std::vector<std::string> commands = std::vector<std::string>());

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
    void adjustAffixWeight(float multiplier, AffixType prefixType, AffixType suffixType);
    void adjustChampionChance(float multiplier);
    void adjustChampionSpawnAmount(float multiplier);
    void adjustChampionSpawnMin(float multiplier);
    void adjustChampionSpawnMax(float multiplier);
    void adjustCommonSpawnAmount(float multiplier);
    void adjustCommonSpawnMin(float multiplier);
    void adjustCommonSpawnMax(float multiplier);
    void adjustExpRequirement(float multiplier);
    void setExpRequirementEquation(std::string value);
    void adjustFactionRepRequirements(float multiplier);
    void adjustGoldDrop(float multiplier);
    void adjustLootAmount(float multiplier, LootSource source = LootSource::All);
    void adjustMonsterClassWeight(float multiplier, MonsterClass monsterClass);
    void adjustSpawnAmount(float multiplier);
    void adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types = std::vector<ItemType>(), std::vector<ItemClass> rarities = std::vector<ItemClass>(), bool isAnd = false);
    void increaseMonsterClassLimit(int limit, MonsterClass monsterClass);
    void removeDifficultyLimits();
    void setDevotionPointsPerShrine(int point);
    void setItemStackLimit(int limit, ItemType type);
    void setMaxDevotionPoints(int point);
    void setMaxLevel(int level);
    void setMonsterClassMaxPlayerLevel(int level, MonsterClass monsterClass);
    void setMonsterClassMinPlayerLevel(int level, MonsterClass monsterClass);
    void setCharModPoints(int value);
    void setDexInc(int value);
    void setStrInc(int value);
    void setIntInc(int value);
    void setLifeInc(int value);
    void setLifeIncDex(int value);
    void setLifeIncInt(int value);
    void setManaInc(int value);
    void adjustSkillModPointPerLevel(int change);
    void setSkillModPointPerLevel(std::string value);

private:
    void _parseCommands(std::vector<std::string> commands);
    template <typename T>
    void _addFileForPreParse();
    void _addFileForPreParse(std::string s);
    void _preParse(int tnum, std::vector<DBRBase*> temps);
    void _preParseWOThreads();
    void _getWorkPart(int& start, int& end);
};