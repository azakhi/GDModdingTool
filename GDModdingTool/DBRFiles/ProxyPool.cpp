#include "ProxyPool.h"

#include "Monster.h"

void ProxyPool::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();

    {
        auto it = _fields.find("ignoreGameBalance");
        if (it != _fields.end()) {
            _ignoreGameBalance = it->second.second->value() == "1";
        }
    }
    {
        auto it = _fields.find("championChance");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _championChance = new NumericField<float>(field->name(), field->value(), false, 0.0f, 100.0f);
            _fieldsOrdered[it->second.first] = _championChance;
            _fields[field->name()] = std::make_pair(it->second.first, _championChance);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("championMax");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _championMax = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _championMax;
            _fields[field->name()] = std::make_pair(it->second.first, _championMax);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("championMin");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _championMin = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _championMin;
            _fields[field->name()] = std::make_pair(it->second.first, _championMin);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("spawnMax");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _spawnMax = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _spawnMax;
            _fields[field->name()] = std::make_pair(it->second.first, _spawnMax);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("spawnMin");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _spawnMin = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _spawnMin;
            _fields[field->name()] = std::make_pair(it->second.first, _spawnMin);
            delete field;
            field = nullptr;
        }
    }

    for (int spawnNum = 1; spawnNum <= 15; spawnNum++) {
        for (int spawnType = 0; spawnType < 2; spawnType++) {
            std::string suffix = spawnType == 0 ? "" : "Champion";

            auto nameIt = _fields.find("name" + suffix + std::to_string(spawnNum));
            auto weightIt = _fields.find("weight" + suffix + std::to_string(spawnNum));
            if (nameIt == _fields.end() || weightIt == _fields.end() || std::stoi(weightIt->second.second->value()) == 0) {
                continue; // Skip if either is missing or weight is 0
            }

            Spawn s;
            s.monster = _fileManager->getFile(nameIt->second.second->value());
            Field* weightField = weightIt->second.second;
            NumericField<int>* weight = new NumericField<int>(weightField->name(), weightField->value(), true);
            s.weight = weight;
            _fieldsOrdered[weightIt->second.first] = weight;
            _fields[weightField->name()] = std::make_pair(weightIt->second.first, weight);
            delete weightField;
            weightField = nullptr;

            {
                auto it = _fields.find("limit" + suffix + std::to_string(spawnNum));
                if (it != _fields.end()) {
                    Field* field = it->second.second;
                    NumericField<int>* newField = new NumericField<int>(field->name(), field->value());
                    s.limit = newField;
                    _fieldsOrdered[it->second.first] = newField;
                    _fields[field->name()] = std::make_pair(it->second.first, newField);
                    delete field;
                    field = nullptr;
                }
            }
            {
                auto it = _fields.find("maxPlayerLevel" + suffix + std::to_string(spawnNum));
                if (it != _fields.end()) {
                    Field* field = it->second.second;
                    NumericField<int>* newField = new NumericField<int>(field->name(), field->value());
                    s.maxPlayerLevel = newField;
                    _fieldsOrdered[it->second.first] = newField;
                    _fields[field->name()] = std::make_pair(it->second.first, newField);
                    delete field;
                    field = nullptr;
                }
            }
            {
                auto it = _fields.find("minPlayerLevel" + suffix + std::to_string(spawnNum));
                if (it != _fields.end()) {
                    Field* field = it->second.second;
                    NumericField<int>* newField = new NumericField<int>(field->name(), field->value());
                    s.minPlayerLevel = newField;
                    _fieldsOrdered[it->second.first] = newField;
                    _fields[field->name()] = std::make_pair(it->second.first, newField);
                    delete field;
                    field = nullptr;
                }
            }

            _spawns[spawnType].push_back(s);
        }
    }

/*
    for (size_t i = 0; i < _fieldsOrdered.size(); i++) {
        Field* field = _fieldsOrdered[i];
        std::string name = field->name();

        if (name == "ignoreGameBalance") {
            _ignoreGameBalance = field->value() == "1";
        }
        else if (name == "championChance") {
            _championChance = new NumericField<float>(name, field->value());
            _fieldsOrdered[i] = _championChance;
            _fields[name] = _championChance;
            delete field;
            field = nullptr;
        }
        else if (name == "championMax") {
            _championMax = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = _championMax;
            _fields[name] = _championMax;
            delete field;
            field = nullptr;
        }
        else if (name == "championMin") {
            _championMin = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = _championMin;
            _fields[name] = _championMin;
            delete field;
            field = nullptr;
        }
        else if (name == "spawnMax") {
            _spawnMax = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = _spawnMax;
            _fields[name] = _spawnMax;
            delete field;
            field = nullptr;
        }
        else if (name == "spawnMin") {
            _spawnMin = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = _spawnMin;
            _fields[name] = _spawnMin;
            delete field;
            field = nullptr;
        }
        else if (name.substr(0, 5) == "limit") {
            size_t num = 0;
            int spawnIndex = 0;

            if (name.substr(5, 8) == "Champion") {
                num = std::stoi(name.substr(13)) - 1;
                spawnIndex = 1;
            }
            else {
                num = std::stoi(name.substr(5)) - 1;
            }

            if (num >= _spawns[spawnIndex].size()) {
                _spawns[spawnIndex].insert(_spawns[spawnIndex].end(), num - _spawns[spawnIndex].size() + 1, Spawn());
            }

            NumericField<int>* limit = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = limit;
            _fields[name] = limit;
            delete field;
            field = nullptr;
            _spawns[spawnIndex][num].limit = limit;
        }
        else if (name.substr(0, 14) == "maxPlayerLevel") {
            size_t num = 0;
            int spawnIndex = 0;

            if (name.substr(14, 8) == "Champion") {
                num = std::stoi(name.substr(22)) - 1;
                spawnIndex = 1;
            }
            else {
                num = std::stoi(name.substr(14)) - 1;
            }

            if (num >= _spawns[spawnIndex].size()) {
                _spawns[spawnIndex].insert(_spawns[spawnIndex].end(), num - _spawns[spawnIndex].size() + 1, Spawn());
            }

            NumericField<int>* maxPlayerLevel = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = maxPlayerLevel;
            _fields[name] = maxPlayerLevel;
            delete field;
            field = nullptr;
            _spawns[spawnIndex][num].maxPlayerLevel = maxPlayerLevel;
        }
        else if (name.substr(0, 14) == "minPlayerLevel") {
            size_t num = 0;
            int spawnIndex = 0;

            if (name.substr(14, 8) == "Champion") {
                num = std::stoi(name.substr(22)) - 1;
                spawnIndex = 1;
            }
            else {
                num = std::stoi(name.substr(14)) - 1;
            }

            if (num >= _spawns[spawnIndex].size()) {
                _spawns[spawnIndex].insert(_spawns[spawnIndex].end(), num - _spawns[spawnIndex].size() + 1, Spawn());
            }

            NumericField<int>* minPlayerLevel = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = minPlayerLevel;
            _fields[name] = minPlayerLevel;
            delete field;
            field = nullptr;
            _spawns[spawnIndex][num].minPlayerLevel = minPlayerLevel;
        }
        else if (name.substr(0, 4) == "name") {
            size_t num = 0;
            int spawnIndex = 0;

            if (name.substr(4, 8) == "Champion") {
                num = std::stoi(name.substr(12)) - 1;
                spawnIndex = 1;
            }
            else {
                num = std::stoi(name.substr(4)) - 1;
            }

            if (num >= _spawns[spawnIndex].size()) {
                _spawns[spawnIndex].insert(_spawns[spawnIndex].end(), num - _spawns[spawnIndex].size() + 1, Spawn());
            }

            _spawns[spawnIndex][num].monster = _fileManager->getFile(field->value());
        }
        else if (name.substr(0, 6) == "weight") {
            size_t num = 0;
            int spawnIndex = 0;

            if (name.substr(6, 8) == "Champion") {
                num = std::stoi(name.substr(14)) - 1;
                spawnIndex = 1;
            }
            else {
                num = std::stoi(name.substr(6)) - 1;
            }

            if (num >= _spawns[spawnIndex].size()) {
                _spawns[spawnIndex].insert(_spawns[spawnIndex].end(), num - _spawns[spawnIndex].size() + 1, Spawn());
            }


            NumericField<int>* weight = new NumericField<int>(name, field->value());
            _fieldsOrdered[i] = weight;
            _fields[name] = weight;
            delete field;
            field = nullptr;
            _spawns[spawnIndex][num].weight = weight;
        }
    }
*/

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