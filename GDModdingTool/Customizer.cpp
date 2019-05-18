#include "Customizer.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>

const std::function<int(std::string)> ParamTypes::Integer = [](std::string s) { return std::stoi(s); };
const std::function<float(std::string)> ParamTypes::Float = [](std::string s) { return std::stof(s); };
const std::function<bool(std::string)> ParamTypes::Boolean = [](std::string s) { return s == "True"; };
const std::function<AffixType(std::string)> ParamTypes::AffixTypeEnum = [](std::string s) {
    if (s == "NoAffix") return AffixType::NoAffix;
    else if (s == "NormalAffix") return AffixType::NormalAffix;
    else if (s == "RareAffix") return AffixType::RareAffix;
    else throw "Invalid Affix";
};
const std::function<LootSource(std::string)> ParamTypes::LootSourceEnum = [](std::string s) {
    if (s == "Chest") return LootSource::Chest;
    else if (s == "Enemy") return LootSource::Enemy;
    else if (s == "All") return LootSource::All;
    else throw "Invalid Affix";
};
const std::function<MonsterClass(std::string)> ParamTypes::MonsterClassEnum = [](std::string s) { return DBRBase::MonsterClassOf(s); };
const std::function<ItemType(std::string)> ParamTypes::ItemTypeEnum = [](std::string s) { return DBRBase::ItemTypeOf(s); };
const std::function<ItemClass(std::string)> ParamTypes::ItemClassEnum = [](std::string s) { return DBRBase::ItemClassOf(s); };
template <typename T>
const std::function<std::vector<T>(std::string, std::function<T(std::string)>)> ParamTypes::Vector = [](std::string s, std::function<T(std::string)> f, std::vector<T> container = std::vector<T>()) {
    std::stringstream ss(s);
    std::string item = "";
    while (std::getline(ss, item, ',')) container.push_back(f(item));
    return container;
};

const std::function<bool(std::string)> ParamTypes::IntegerValidator = [](std::string s) { try { std::stoi(s); return true; } catch (...) { return false; } };
const std::function<bool(std::string)> ParamTypes::FloatValidator = [](std::string s) { try { std::stof(s); return true; } catch (...) { return false; } };
const std::function<bool(std::string)> ParamTypes::BooleanValidator = [](std::string s) { return s == "True" || s == "False"; };
const std::function<bool(std::string)> ParamTypes::AffixTypeEnumValidator = [](std::string s) { try { ParamTypes::AffixTypeEnum(s); return true; } catch (...) { return false; } };
const std::function<bool(std::string)> ParamTypes::LootSourceEnumValidator = [](std::string s) { try { ParamTypes::LootSourceEnum(s); return true; } catch (...) { return false; } };
const std::function<bool(std::string)> ParamTypes::MonsterClassEnumValidator = [](std::string s) { return DBRBase::MonsterClassOf(s) != MonsterClass::NoClass; };
const std::function<bool(std::string)> ParamTypes::ItemTypeEnumValidator = [](std::string s) { return DBRBase::ItemTypeOf(s) != ItemType::TypeNone; };
const std::function<bool(std::string)> ParamTypes::ItemClassEnumValidator = [](std::string s) { return DBRBase::ItemClassOf(s) != ItemClass::ClassNone; };
const std::function<bool(std::string, std::function<bool(std::string)>)> ParamTypes::VectorValidator = [](std::string s, std::function<bool(std::string)> f) {
    std::stringstream ss(s);
    std::string item = "";
    while (std::getline(ss, item, ',')) if (!f(item)) return false;
    return true;
};

Customizer::Customizer(FileManager* fileManager, std::vector<std::string> commands/* = std::vector<std::string>()*/) {
    _fileManager = fileManager;
    _remainingWorkStart = 0;
    _threadProgress.resize(THREAD_COUNT, 0);
    _isThreadDone.resize(THREAD_COUNT, false);
    _progressTotal = 0;

    _commandMap["AdjustAffixWeight"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator, ParamTypes::AffixTypeEnumValidator, ParamTypes::AffixTypeEnumValidator }),
        [this](std::vector<std::string> params) { adjustAffixWeight(ParamTypes::Float(params[0]), ParamTypes::AffixTypeEnum(params[1]), ParamTypes::AffixTypeEnum(params[2])); });
    _commandMap["AdjustChampionChance"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustChampionChance(ParamTypes::Float(params[0])); });
    _commandMap["AdjustChampionSpawnAmount"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustChampionSpawnAmount(ParamTypes::Float(params[0])); });
    _commandMap["AdjustCommonSpawnAmount"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustCommonSpawnAmount(ParamTypes::Float(params[0])); });
    _commandMap["AdjustExpRequirement"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustExpRequirement(ParamTypes::Float(params[0])); });
    _commandMap["AdjustFactionRepRequirements"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustFactionRepRequirements(ParamTypes::Float(params[0])); });
    _commandMap["AdjustGoldDrop"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustGoldDrop(ParamTypes::Float(params[0])); });
    _commandMap["AdjustLifeIncrement"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustLifeIncrement(ParamTypes::Float(params[0])); });
    _commandMap["AdjustLootAmount"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator, ParamTypes::LootSourceEnumValidator }),
        [this](std::vector<std::string> params) { adjustLootAmount(ParamTypes::Float(params[0]), ParamTypes::LootSourceEnum(params[1])); });
    _commandMap["AdjustMonsterClassWeight"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator, ParamTypes::MonsterClassEnumValidator }),
        [this](std::vector<std::string> params) { adjustMonsterClassWeight(ParamTypes::Float(params[0]), ParamTypes::MonsterClassEnum(params[1])); });
    _commandMap["AdjustSpawnAmount"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::FloatValidator }),
        [this](std::vector<std::string> params) { adjustSpawnAmount(ParamTypes::Float(params[0])); });
    _commandMap["AdjustSpecificLootAmount"] = Command(std::vector<std::function<bool(std::string)>>(
            { ParamTypes::FloatValidator, [](std::string s) {return ParamTypes::VectorValidator(s, ParamTypes::ItemTypeEnumValidator); },
            [](std::string s) {return ParamTypes::VectorValidator(s, ParamTypes::ItemClassEnumValidator); }, ParamTypes::BooleanValidator }
        ),
        [this](std::vector<std::string> params) { adjustSpecificLootAmount(
            ParamTypes::Float(params[0]), ParamTypes::Vector<ItemType>(params[1], ParamTypes::ItemTypeEnum),
            ParamTypes::Vector<ItemClass>(params[2], ParamTypes::ItemClassEnum), ParamTypes::Boolean(params[3])
        ); });
    _commandMap["IncreaseMonsterClassLimit"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::IntegerValidator, ParamTypes::MonsterClassEnumValidator }),
        [this](std::vector<std::string> params) { increaseMonsterClassLimit(ParamTypes::Integer(params[0]), ParamTypes::MonsterClassEnum(params[1])); });
    _commandMap["RemoveDifficultyLimits"] = Command(std::vector<std::function<bool(std::string)>>(), [this](std::vector<std::string> params) { removeDifficultyLimits(); });
    _commandMap["SetDevotionPointsPerShrine"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::IntegerValidator }),
        [this](std::vector<std::string> params) { setDevotionPointsPerShrine(ParamTypes::Integer(params[0])); });
    _commandMap["SetItemStackLimit"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::IntegerValidator, ParamTypes::ItemTypeEnumValidator }),
        [this](std::vector<std::string> params) { setItemStackLimit(ParamTypes::Integer(params[0]), ParamTypes::ItemTypeEnum(params[1])); });
    _commandMap["SetMaxDevotionPoints"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::IntegerValidator }),
        [this](std::vector<std::string> params) { setMaxDevotionPoints(ParamTypes::Integer(params[0])); });
    _commandMap["SetMaxLevel"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::IntegerValidator }),
        [this](std::vector<std::string> params) { setMaxLevel(ParamTypes::Integer(params[0])); });
    _commandMap["SetMonsterClassMaxPlayerLevel"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::IntegerValidator, ParamTypes::MonsterClassEnumValidator }),
        [this](std::vector<std::string> params) { setMonsterClassMaxPlayerLevel(ParamTypes::Integer(params[0]), ParamTypes::MonsterClassEnum(params[1])); });
    _commandMap["SetMonsterClassMinPlayerLevel"] = Command(std::vector<std::function<bool(std::string)>>({ ParamTypes::IntegerValidator, ParamTypes::MonsterClassEnumValidator }),
        [this](std::vector<std::string> params) { setMonsterClassMinPlayerLevel(ParamTypes::Integer(params[0]), ParamTypes::MonsterClassEnum(params[1])); });

    _parseCommands(commands);
}

void Customizer::_parseCommands(std::vector<std::string> commands) {
    for (std::string s : commands) {
        std::stringstream ss(s);
        std::vector<std::string> args;
        std::string item = "";
        while (std::getline(ss, item, ' ')) {
            args.push_back(item);
        }

        if (args.size() > 0) {
            if (_commandMap.find(args[0]) == _commandMap.end()) {
                log_error << "Unknown command: " << s << "\n";
                continue;
            }

            const Command& c = _commandMap[args[0]];
            std::string commandName = args[0];
            args.erase(args.begin());
            if (!c.isValid(args)) {
                log_error << "Invalid parameters: " << s << "\n";
                continue;
            }
            
            log_info << "Running command: " << commandName << "\n";
            c.method(args);
        }
    }
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
    _progressTotal = (int)preParseFiles.size();
    _remainingWorkStart = 0;
    for (size_t i = 0; i < _threadProgress.size(); i++) {
        auto t = std::thread(&Customizer::_preParse, this, (int)i, preParseFiles);
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
        std::vector<DBRBase*> playerFile = fmCopy->getFiles("experiencelevelcontrol.tpl");
        for (auto temp : playerFile) {
            temp->modifyField("maxDevotionPoints", [point](std::string str) -> std::string {
                return std::to_string(point);
            });
        }

        std::vector<DBRBase*> gameEngineFile = fmCopy->getFiles("gameengine.tpl");
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
        std::vector<DBRBase*> temps = fmCopy->getFiles<ProxyPool>();
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
        std::vector<DBRBase*> temps = fmCopy->getFiles<ProxyPool>();
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
        std::vector<DBRBase*> temps = fmCopy->getFiles<ProxyPool>();
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
            std::vector<DBRBase*> temps = fmCopy->getFiles<FixedItemLoot>();
            for (auto temp : temps) {
                FixedItemLoot* fil = (FixedItemLoot*)temp;
                fil->adjustLootAmount(multiplier);
            }
        }

        if (source == LootSource::All || source == LootSource::Enemy) {
            std::vector<DBRBase*> temps = fmCopy->getFiles<Monster>();
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
        std::vector<DBRBase*> lmttemps = fmCopy->getFiles<LootMasterTable>();
        for (auto temp : lmttemps) {
            LootMasterTable* lmt = (LootMasterTable*)temp;
            lmt->adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
        }

        std::vector<DBRBase*> dyntemps = fmCopy->getFiles<DynWeightAffixTable>();
        for (auto temp : dyntemps) {
            DynWeightAffixTable* dyn = (DynWeightAffixTable*)temp;
            dyn->adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
        }

        std::vector<DBRBase*> filtemps = fmCopy->getFiles<FixedItemLoot>();
        for (auto temp : filtemps) {
            FixedItemLoot* fil = (FixedItemLoot*)temp;
            fil->updateFromChild();
            fil->adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
        }

        std::vector<DBRBase*> montemps = fmCopy->getFiles<Monster>();
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
        std::vector<DBRBase*> proxies = fmCopy->getFiles("proxy.tpl");
        for (auto temp : proxies) {
            temp->modifyField("difficultyLimitsFile", [](std::string str) -> std::string {
                return "records/proxies/limit_unlimited.dbr";
            });

            temp->addFieldIfNotExists("difficultyLimitsFile", "records/proxies/limit_unlimited.dbr");
        }

        std::vector<DBRBase*> proxyambushes = fmCopy->getFiles("proxyambush.tpl");
        for (auto temp : proxyambushes) {
            temp->modifyField("difficultyLimitsFile", [](std::string str) -> std::string {
                return "records/proxies/limit_unlimited.dbr";
            });

            temp->addFieldIfNotExists("difficultyLimitsFile", "records/proxies/limit_unlimited.dbr");
        }
    };
    _tasks.push_back(f);
}

void Customizer::adjustMonsterClassWeight(float multiplier, MonsterClass monsterClass) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, multiplier]() {
        std::vector<DBRBase*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->adjustMonsterClassWeight(monsterClass, multiplier);
        }
    };
    _tasks.push_back(f);
}

void Customizer::increaseMonsterClassLimit(int limit, MonsterClass monsterClass) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, limit]() {
        std::vector<DBRBase*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->increaseMonsterClassLimit(monsterClass, limit);
        }
    };
    _tasks.push_back(f);
}

void Customizer::setMonsterClassMaxPlayerLevel(int level, MonsterClass monsterClass) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, level]() {
        std::vector<DBRBase*> temps = fmCopy->getFiles<ProxyPool>();
        for (auto temp : temps) {
            ProxyPool* pool = (ProxyPool*)temp;
            pool->setMonsterClassMaxPlayerLevel(monsterClass, level);
        }
    };
    _tasks.push_back(f);
}

void Customizer::setMonsterClassMinPlayerLevel(int level, MonsterClass monsterClass) {
    _addFileForPreParse<Monster>();
    _addFileForPreParse<ProxyPool>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, monsterClass, level]() {
        std::vector<DBRBase*> temps = fmCopy->getFiles<ProxyPool>();
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
        std::vector<DBRBase*> temps = fmCopy->getFiles("gameengine.tpl");
        for (auto temp : temps) {
            temp->modifyField(fields, [multiplier](std::string str) -> std::string {
                float value = std::stof(str);
                return std::to_string((int)(value * multiplier));
            });
        }
    };
    _tasks.push_back(f);
}

void Customizer::setItemStackLimit(int limit, ItemType type) {
    _addFileForPreParse<ItemBase>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f1 = [fmCopy, type, limit]() {
        std::vector<DBRBase*> temps = fmCopy->getFiles<ItemBase>();
        for (auto temp : temps) {
            ItemBase* item = (ItemBase*)temp;
            if (item->itemType() == type)
            item->setMaxStackSize(limit);
        }
    };
    _tasks.push_back(f1);

    _addFileForPreParse("gameengine.tpl");
    std::function<void()> f2 = [fmCopy, limit]() {
        std::vector<DBRBase*> gameEngineFile = fmCopy->getFiles("gameengine.tpl");
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

void Customizer::adjustAffixWeight(float multiplier, AffixType prefixType, AffixType suffixType) {
    _addFileForPreParse<DynWeightAffixTable>();

    FileManager* fmCopy = _fileManager;
    std::function<void()> f = [fmCopy, multiplier, prefixType, suffixType]() {
        std::vector<DBRBase*> temps = fmCopy->getFiles<DynWeightAffixTable>();
        for (auto temp : temps) {
            DynWeightAffixTable* dynTable = (DynWeightAffixTable*)temp;
            dynTable->adjustAffixWeight(multiplier, prefixType, suffixType);
        }
    };
    _tasks.push_back(f);
}

void Customizer::_preParse(int tnum, std::vector<DBRBase*> temps) {
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