#include "Customizer.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>

#include <iostream>

Customizer::Customizer(FileManager* fileManager) {
    _fileManager = fileManager;
    _remainingWorkStart = 0;
    _threadProgress.resize(THREAD_COUNT, 0);
    _isThreadDone.resize(THREAD_COUNT, false);
    _progressTotal = 0;
}

template <typename T>
void Customizer::_addFileForPreParse() {
    if (std::find(_preParseFiles.begin(), _preParseFiles.end(), std::type_index(typeid(T))) == _preParseFiles.end()) {
        _preParseFiles.push_back(std::type_index(typeid(T)));
    }
}

void Customizer::_addFileForPreParse(std::string s) {
    if (std::find(_preParseFilesStr.begin(), _preParseFilesStr.end(), s) == _preParseFilesStr.end()) {
        _preParseFilesStr.push_back(s);
    }
}

void Customizer::preParse() {
    auto preParseFiles = _fileManager->getFiles(_preParseFiles);
    auto preParseFilesStr = _fileManager->getFiles(_preParseFilesStr);
    preParseFiles.insert(preParseFiles.end(), preParseFilesStr.begin(), preParseFilesStr.end());
    _progressTotal = preParseFiles.size();
    _remainingWorkStart = 0;
    for (size_t i = 0; i < _threadProgress.size(); i++) {
        auto t = std::thread(&Customizer::_preParse, this, i, preParseFiles);
        t.detach();
    }
}

void Customizer::runTasks() {
    for (auto& f : _tasks) {
        f();
    }
}

void Customizer::adjustLifeIncrement(float multiplier) {
    _addFileForPreParse("experiencelevelcontrol.tpl");
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier]() {
        fmCopy->modifyField("experiencelevelcontrol.tpl", "lifeIncrement", [multiplier](std::string str) -> std::string {
            float value = std::stof(str);
            return std::to_string((int)(value * multiplier));
        });
    };
    _tasks.push_back(f);
}

void Customizer::setMaxDevotionPoints(int point) {
    _addFileForPreParse("experiencelevelcontrol.tpl");
    _addFileForPreParse("gameengine.tpl");
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, point]() {
        std::vector<Template*> playerFile = fmCopy->getFiles("experiencelevelcontrol.tpl");
        for (auto temp : playerFile) {
            temp->modifyField("maxDevotionPoints", [point](std::string str) -> std::string {
                return std::to_string(point);
            });
        }

        std::vector<Template*> gameEngineFile = fmCopy->getFiles("gameengine.tpl");
        for (auto temp : gameEngineFile) {
            temp->modifyField("playerDevotionCap", [point](std::string str) -> std::string {
                return std::to_string(point);
            });
        }
    };
    _tasks.push_back(f);
}

void Customizer::setMaxLevel(int level) {
    _addFileForPreParse("experiencelevelcontrol.tpl");
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, level]() {
        fmCopy->modifyField("experiencelevelcontrol.tpl", "maxPlayerLevel", [level](std::string str) -> std::string {
            return std::to_string(level);
        });
    };
    _tasks.push_back(f);
}

void Customizer::adjustGoldDrop(float multiplier) {
    _addFileForPreParse("goldgenerator.tpl");
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier]() {
        std::vector<std::string> fields;
        fields.push_back("goldValueMin");
        fields.push_back("goldValueMax");
        fmCopy->modifyField("goldgenerator.tpl", fields, [multiplier](std::string str) -> std::string {
            float value = std::stof(str);
            return std::to_string((int)(value * multiplier));
        });

        fields.clear();
        fields.push_back("goldAmountEquation");
        fields.push_back("goldAmountEquation2");
        fmCopy->modifyField("goldgenerator.tpl", fields, [multiplier](std::string str) -> std::string {
            std::ostringstream os;
            os << std::fixed << std::setprecision(1) << "(" << str << ") * " << multiplier;
            return os.str();
        });
    };
    _tasks.push_back(f);
}

void Customizer::adjustExpRequirement(float multiplier) {
    _addFileForPreParse("experiencelevelcontrol.tpl");
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier]() {
        fmCopy->modifyField("experiencelevelcontrol.tpl", "experienceLevelEquation", [multiplier](std::string str) -> std::string {
            std::ostringstream os;
            os << std::fixed << std::setprecision(1) << "(" << str << ") * " << multiplier;
            return os.str();
        });
    };
    _tasks.push_back(f);
}

void Customizer::setDevotionPointsPerShrine(int point) {
    _addFileForPreParse("staticshrine.tpl");
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, point]() {
        fmCopy->modifyField("staticshrine.tpl", "devotionPoints", [point](std::string str) -> std::string {
            return std::to_string(point);
        });
    };
    _tasks.push_back(f);
}

void Customizer::adjustCommonSpawnAmount(float multiplier) {
    _addFileForPreParse<ProxyPool>();
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier]() {
        std::vector<Template*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->adjustSpawnAmount(multiplier);
        }
    };
    _tasks.push_back(f);
}

void Customizer::adjustChampionSpawnAmount(float multiplier) {
    _addFileForPreParse<ProxyPool>();
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier]() {
        std::vector<Template*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->adjustChampionSpawnAmount(multiplier);
        }
    };
    _tasks.push_back(f);
}

void Customizer::adjustChampionChance(float multiplier) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier]() {
        std::vector<Template*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->adjustChampionChance(multiplier);
        }
    };
    _tasks.push_back(f);
}

void Customizer::adjustSpawnAmount(float multiplier) {
    adjustCommonSpawnAmount(multiplier);
    adjustChampionSpawnAmount(multiplier);
}

void Customizer::adjustLootAmount(float multiplier, LootSource source/* = LootSource::All*/) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<FixedItemLoot>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier, source]() {
        if (source == LootSource::All || source == LootSource::Chest) {
            std::vector<Template*> temps = fmCopy->getFiles<FixedItemLoot>();
            for (auto temp : temps) {
                FixedItemLoot* fil = (FixedItemLoot*)temp;
                fil->adjustLootAmount(multiplier);
            }
        }

        if (source == LootSource::All || source == LootSource::Enemy) {
            std::vector<Template*> temps = fmCopy->getFiles<Monster>();
            for (auto temp : temps) {
                Monster* mon = (Monster*)temp;
                mon->adjustLootAmount(multiplier);
            }
        }
    };
    _tasks.push_back(f);
}

void Customizer::adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types/* = std::vector<ItemType>()*/,
    std::vector<ItemClass> rarities/* = std::vector<ItemClass>()*/, bool isAnd/* = false*/) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<FixedItemLoot>();
    _addFileForPreParse<LootMasterTable>();
    _addFileForPreParse<DynWeightAffixTable>();
    _addFileForPreParse<LevelTable>();
    _addFileForPreParse<ItemBase>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier, types, rarities, isAnd]() {
        std::vector<Template*> lmttemps = fmCopy->getFiles<LootMasterTable>();
        for (auto temp : lmttemps) {
            LootMasterTable* lmt = (LootMasterTable*)temp;
            lmt->adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
        }

        std::vector<Template*> dyntemps = fmCopy->getFiles<DynWeightAffixTable>();
        for (auto temp : dyntemps) {
            DynWeightAffixTable* dyn = (DynWeightAffixTable*)temp;
            dyn->adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
        }

        std::vector<Template*> filtemps = fmCopy->getFiles<FixedItemLoot>();
        for (auto temp : filtemps) {
            FixedItemLoot* fil = (FixedItemLoot*)temp;
            fil->updateFromChild();
            fil->adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
        }

        std::vector<Template*> montemps = fmCopy->getFiles<Monster>();
        for (auto temp : montemps) {
            Monster* mon = (Monster*)temp;
            mon->updateFromChild();
            mon->adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
        }
    };
    _tasks.push_back(f);
}

void Customizer::removeDifficultyLimits() {
    _addFileForPreParse("proxy.tpl");
    _addFileForPreParse("proxyambush.tpl");

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy]() {
        std::vector<Template*> proxies = fmCopy->getFiles("proxy.tpl");
        for (auto temp : proxies) {
            temp->modifyField("difficultyLimitsFile", [](std::string str) -> std::string {
                return "records/proxies/limit_unlimited.dbr";
            });

            temp->addFieldIfNotExists("difficultyLimitsFile", "records/proxies/limit_unlimited.dbr");
        }

        std::vector<Template*> proxyambushes = fmCopy->getFiles("proxyambush.tpl");
        for (auto temp : proxyambushes) {
            temp->modifyField("difficultyLimitsFile", [](std::string str) -> std::string {
                return "records/proxies/limit_unlimited.dbr";
            });

            temp->addFieldIfNotExists("difficultyLimitsFile", "records/proxies/limit_unlimited.dbr");
        }
    };
    _tasks.push_back(f);
}

void Customizer::adjustMonsterClassWeight(MonsterClass monsterClass, float multiplier) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, multiplier]() {
        std::vector<Template*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->adjustMonsterClassWeight(monsterClass, multiplier);
        }
    };
    _tasks.push_back(f);
}

void Customizer::increaseMonsterClassLimit(MonsterClass monsterClass, int limit) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, limit]() {
        std::vector<Template*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->increaseMonsterClassLimit(monsterClass, limit);
        }
    };
    _tasks.push_back(f);
}

void Customizer::setMonsterClassMaxPlayerLevel(MonsterClass monsterClass, int level) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, level]() {
        std::vector<Template*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->setMonsterClassMaxPlayerLevel(monsterClass, level);
        }
    };
    _tasks.push_back(f);
}

void Customizer::setMonsterClassMinPlayerLevel(MonsterClass monsterClass, int level) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, level]() {
        std::vector<Template*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->setMonsterClassMinPlayerLevel(monsterClass, level);
        }
    };
    _tasks.push_back(f);
}

void Customizer::adjustFactionRepRequirements(float multiplier) {
    _addFileForPreParse("gameengine.tpl");
    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier]() {
        std::vector<std::string> fields;
        fields.push_back("factionValue1");
        fields.push_back("factionValue2");
        fields.push_back("factionValue3");
        fields.push_back("factionValue5");
        fields.push_back("factionValue6");
        fields.push_back("factionValue7");
        fields.push_back("factionValue8");
        std::vector<Template*> temps = fmCopy->getFiles("gameengine.tpl");
        for (auto temp : temps) {
            temp->modifyField(fields, [multiplier](std::string str) -> std::string {
                float value = std::stof(str);
                return std::to_string((int)(value * multiplier));
            });
        }
    };
    _tasks.push_back(f);
}

void Customizer::setItemStackLimit(ItemType type, int limit) {
    _addFileForPreParse<ItemBase>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f1 = [fmCopy, type, limit]() {
        std::vector<Template*> temps = fmCopy->getFiles<ItemBase>();
        for (auto temp : temps) {
            ItemBase* item = (ItemBase*)temp;
            if (item->itemType() == type)
            item->setMaxStackSize(limit);
        }
    };
    _tasks.push_back(f1);

    _addFileForPreParse("gameengine.tpl");
    std::function<void()> f2 = [fmCopy, limit]() {
        std::vector<Template*> gameEngineFile = fmCopy->getFiles("gameengine.tpl");
        for (auto temp : gameEngineFile) {
            std::vector<std::string> fields;
            fields.push_back("potionStackLimit");
            fields.push_back("scrollStackLimit");
            fields.push_back("itemMaxStackSize");
            fields.push_back("questItemStackLimit");
            temp->modifyField(fields, [limit](std::string str) -> std::string {
                return std::to_string(limit);
            });
        }
    };
    _tasks.push_back(f2);
}

void Customizer::_preParse(int tnum, std::vector<Template*> temps) {
    _isThreadDone[tnum] = false;
    _threadProgress[tnum] = 0;

    int start, end;
    _getWorkPart(start, end);
    while (start < end) {
        for (int i = start; i < end; i++) {
            temps[i]->parse();
            _threadProgress[tnum]++;
        }
        _getWorkPart(start, end);
    }

    _isThreadDone[tnum] = true;
}

void Customizer::_preParseWOThreads() {
    auto preParseFiles = _fileManager->getFiles(_preParseFiles);

    for (auto f : preParseFiles) {
        f->parse();
    }
}

void Customizer::_getWorkPart(int& start, int& end) {
    std::lock_guard<std::mutex> lock(_workPartMutex);
    int size = std::min<int>(_progressTotal - _remainingWorkStart, std::max<int>(50, std::min<int>(200, (_progressTotal - _remainingWorkStart) / THREAD_COUNT)));
    start = _remainingWorkStart;
    _remainingWorkStart += size;
    end = _remainingWorkStart;
}