#include "ProxyPool.h"

#include "Monster.h"

void ProxyPool::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();

    if (_fieldMap["ignoreGameBalance"] != nullptr) {
        _ignoreGameBalance = _fieldMap["ignoreGameBalance"]->value() == "1";
    }

    if (_fieldMap["championChance"] != nullptr) {
        Field* field = _fieldMap["championChance"];
        _championChance = new NumericField<float>(field->name(), field->value(), false, 0.0f, 100.0f);
        _fieldMap["championChance"] = _championChance;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["championMax"] != nullptr) {
        Field* field = _fieldMap["championMax"];
        _championMax = new NumericField<int>(field->name(), field->value());
        _fieldMap["championMax"] = _championChance;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["championMin"] != nullptr) {
        Field* field = _fieldMap["championMin"];
        _championMax = new NumericField<int>(field->name(), field->value());
        _fieldMap["championMin"] = _championChance;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["spawnMax"] != nullptr) {
        Field* field = _fieldMap["spawnMax"];
        _championMax = new NumericField<int>(field->name(), field->value());
        _fieldMap["spawnMax"] = _championChance;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["spawnMin"] != nullptr) {
        Field* field = _fieldMap["spawnMin"];
        _championMax = new NumericField<int>(field->name(), field->value());
        _fieldMap["spawnMin"] = _championChance;
        delete field;
        field = nullptr;
    }

    for (int spawnNum = 1; spawnNum <= 15; spawnNum++) {
        for (int spawnType = 0; spawnType < 2; spawnType++) {
            std::string suffix = spawnType == 0 ? "" : "Champion";

            Field* nameField = _fieldMap["name" + suffix + std::to_string(spawnNum)];
            Field* weightField = _fieldMap["weight" + suffix + std::to_string(spawnNum)];
            if (nameField == nullptr || weightField == nullptr || std::stoi(weightField->value()) == 0) {
                continue; // Skip if either is missing or weight is 0
            }

            Spawn s;
            s.monster = _fileManager->getFile(nameField->value());
            NumericField<int>* weight = new NumericField<int>(weightField->name(), weightField->value(), true);
            s.weight = weight;
            _fieldMap[weightField->name()] = weight;
            delete weightField;
            weightField = nullptr;

            std::string fieldName = "limit" + suffix + std::to_string(spawnNum);
            if (_fieldMap[fieldName] != nullptr) {
                Field* field = _fieldMap[fieldName];
                NumericField<int>* newField = new NumericField<int>(field->name(), field->value());
                s.limit = newField;
                _fieldMap[fieldName] = newField;
                delete field;
                field = nullptr;
            }

            fieldName = "maxPlayerLevel" + suffix + std::to_string(spawnNum);
            if (_fieldMap[fieldName] != nullptr) {
                Field* field = _fieldMap[fieldName];
                NumericField<int>* newField = new NumericField<int>(field->name(), field->value());
                s.limit = newField;
                _fieldMap[fieldName] = newField;
                delete field;
                field = nullptr;
            }

            fieldName = "minPlayerLevel" + suffix + std::to_string(spawnNum);
            if (_fieldMap[fieldName] != nullptr) {
                Field* field = _fieldMap[fieldName];
                NumericField<int>* newField = new NumericField<int>(field->name(), field->value());
                s.limit = newField;
                _fieldMap[fieldName] = newField;
                delete field;
                field = nullptr;
            }

            _spawns[spawnType].push_back(s);
        }
    }

    _isParsed = true;
}

void ProxyPool::adjustSpawnAmount(float multiplier) {
    if (_spawnMax != nullptr) _spawnMax->setModifiedValue(std::max(0.0f, _spawnMax->numericValue() * multiplier));
    if (_spawnMin != nullptr) _spawnMin->setModifiedValue(std::max(0.0f, _spawnMin->numericValue() * multiplier));
}

void ProxyPool::adjustChampionSpawnAmount(float multiplier) {
    if (_championMax != nullptr) _championMax->setModifiedValue(std::max(0.0f, _championMax->numericValue() * multiplier));
    if (_championMin != nullptr) _championMin->setModifiedValue(std::max(0.0f, _championMin->numericValue() * multiplier));
}

void ProxyPool::adjustChampionChance(float multiplier) {
    if (_championChance != nullptr) _championChance->setModifiedValue(_championChance->numericValue() * multiplier);
}

void ProxyPool::adjustMonsterClassWeight(MonsterClass monsterClass, float multiplier) {
    for (auto& s : _spawns[0]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.weight != nullptr) s.weight->setModifiedValue(s.weight->numericValue() * multiplier);
        }
    }

    for (auto& s : _spawns[1]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.weight != nullptr) s.weight->setModifiedValue(s.weight->numericValue() * multiplier);
        }
    }
}

void ProxyPool::increaseMonsterClassLimit(MonsterClass monsterClass, int limit) {
    for (auto& s : _spawns[0]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.limit != nullptr) s.limit->setModifiedValue((float)(s.limit->numericValue() + limit));
        }
    }

    for (auto& s : _spawns[1]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.limit != nullptr) s.limit->setModifiedValue((float)(s.limit->numericValue() + limit));
        }
    }
}

void ProxyPool::setMonsterClassMaxPlayerLevel(MonsterClass monsterClass, int limit) {
    for (auto& s : _spawns[0]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.maxPlayerLevel != nullptr) s.maxPlayerLevel->setModifiedValue((float)std::max<int>(0, limit));
        }
    }

    for (auto& s : _spawns[1]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.maxPlayerLevel != nullptr) s.maxPlayerLevel->setModifiedValue((float)std::max<int>(0, limit));
        }
    }
}

void ProxyPool::setMonsterClassMinPlayerLevel(MonsterClass monsterClass, int limit) {
    for (auto& s : _spawns[0]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.minPlayerLevel != nullptr) s.minPlayerLevel->setModifiedValue((float)std::max<int>(0, limit));
        }
    }

    for (auto& s : _spawns[1]) {
        if (s.monster == nullptr) continue;
        if (((Monster*)s.monster)->monsterClass() == monsterClass) {
            if (s.minPlayerLevel != nullptr) s.minPlayerLevel->setModifiedValue((float)std::max<int>(0, limit));
        }
    }
}

bool ProxyPool::_isRatioPreserved() const {
    for (int i = 0; i < 2; i++) {
        if (_spawns[i].size() > 1) {
            float ratio = -1;
            for (const auto& s : _spawns[i]) {
                int modifiedWeight = s.weight->numericModifiedValue();
                if (modifiedWeight == 0) {
                    if (s.weight->numericValue() == 0) {
                        continue;
                    }
                    else {
                        return false;
                    }
                }

                float r = s.weight->numericValue() / (float)modifiedWeight;
                if (ratio < 0) {
                    ratio = r;
                }
                else if (ratio != r) {
                    return false;
                }
            }
        }
    }

    return true;
}