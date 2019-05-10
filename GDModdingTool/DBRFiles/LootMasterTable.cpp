#include "LootMasterTable.h"

void LootMasterTable::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();
    std::vector<Loot> loots;
    for (int lootNum = 1; lootNum <= 100; lootNum++) {
        auto nameIt = _fields.find("lootName" + std::to_string(lootNum));
        if (nameIt == _fields.end()) { // Not sure if game also stops at first empty
            break;
        }

        auto weightIt = _fields.find("lootWeight" + std::to_string(lootNum));
        if (weightIt == _fields.end() || std::stoi(weightIt->second.second->value()) == 0) {
            continue; // Skip if missing or weight is 0
        }

        Loot l;
        l.name = nameIt->second.second;
        l.file = _fileManager->getFile(l.name->value());

        Field* weightField = weightIt->second.second;
        NumericField<int>* weight = new NumericField<int>(weightField->name(), weightField->value(), true);
        l.weight = weight;
        _fieldsOrdered[weightIt->second.first] = weight;
        _fields[weightField->name()] = std::make_pair(weightIt->second.first, weight);
        delete weightField;
        weightField = nullptr;
        loots.push_back(l);
    }

    if (loots.size() > 0) {
        _lootable = Lootable(loots);
    }

/*
    std::vector<int> lootCounts;
    std::vector<Loot> loots;
    int lootCount = 0;
    for (size_t i = 0; i < _fieldsOrdered.size(); i++) {
        Field* field = _fieldsOrdered[i];
        std::string name = field->name();

        if (name.substr(0, 8) == "lootName") {
            lootCount++;
            size_t num = (size_t)std::stoi(name.substr(8)) - 1;
            if (num >= loots.size()) {
                loots.insert(loots.end(), num - loots.size() + 1, Loot());
            }

            loots[num].file = _fileManager->getFile(field->value());
            loots[num].type = DBRBase::TypeOf(loots[num].file);
            loots[num].rarity = DBRBase::RarityOf(loots[num].file);
            loots[num].name = field;
        }
        else if (name.substr(0, 10) == "lootWeight") {
            size_t num = (size_t)std::stoi(name.substr(10)) - 1;
            if (num >= loots.size()) {
                loots.insert(loots.end(), num - loots.size() + 1, Loot());
            }

            NumericField<int>* weight = new NumericField<int>(name, field->value());
            loots[num].weight = weight;
            _fieldsOrdered[i] = weight;
            _fields[name] = weight;
            delete field;
            field = nullptr;
        }
    }

    // Remove unnecessary loots
    loots.erase(loots.begin() + lootCount, loots.end());
    _lootable = Lootable(loots);
*/

    _isParsed = true;
}

void LootMasterTable::adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types/* = std::vector<ItemType>()*/,
    std::vector<ItemClass> rarities/* = std::vector<ItemClass>()*/, bool isAnd/* = false*/) {
    parse();
    _lootable.adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
}