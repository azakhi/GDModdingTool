#include "LootMasterTable.h"

void LootMasterTable::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();
    std::vector<Loot> loots;
    for (int lootNum = 1; lootNum <= 100; lootNum++) {
        Field* nameField = _fieldMap["lootName" + std::to_string(lootNum)];
        if (nameField == nullptr) { // Not sure if game also stops at first empty
            break;
        }

        Field* weightField = _fieldMap["lootWeight" + std::to_string(lootNum)];
        if (weightField == nullptr || std::stoi(weightField->value()) == 0) {
            continue; // Skip if missing or weight is 0
        }

        Loot l;
        l.name = nameField;
        l.file = _fileManager->getFile(l.name->value());

        NumericField<int>* weight = new NumericField<int>(weightField->name(), weightField->value(), true);
        l.weight = weight;
        _fieldMap[weightField->name()] = weight;
        delete weightField;
        weightField = nullptr;
        loots.push_back(l);
    }

    if (loots.size() > 0) {
        _lootable = Lootable(loots);
    }

    _isParsed = true;
}

void LootMasterTable::adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types/* = std::vector<ItemType>()*/,
    std::vector<ItemClass> rarities/* = std::vector<ItemClass>()*/, bool isAnd/* = false*/) {
    parse();
    _lootable.adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
}