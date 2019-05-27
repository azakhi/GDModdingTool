#include "Monster.h"

void Monster::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();
    _modifiedLootAmount = 1.0f;

    if (_fieldMap["monsterClassification"] != nullptr) {
        _monsterClass = DBRBase::MonsterClassOf(_fieldMap["monsterClassification"]->value());
    }

    if (_fieldMap["factions"] != nullptr) {
        _faction = _fileManager->getFaction(_fieldMap["factions"]->value());
    }

    std::vector<std::string> itemSlots({ "Head", "Chest", "Shoulders", "Hands", "Legs", "Feet", "RightHand", "LeftHand", "Finger1", "Finger2", "Misc1", "Misc2", "Misc3" });
    for (const auto& slot : itemSlots) {
        // Check for chance first
        Field* chanceField = _fieldMap["chanceToEquip" + slot];
        if (chanceField == nullptr || std::stof(chanceField->value()) < FLT_MIN) {
            continue; // No need to add
        }

        // Find name-weight matches
        std::vector<Loot> loots;
        for (int i = 1; i <= 6; i++) {
            Field* nameField = _fieldMap["loot" + slot + "Item" + std::to_string(i)];
            Field* weightField = _fieldMap["chanceToEquip" + slot + "Item" + std::to_string(i)];
            if (nameField == nullptr || weightField == nullptr || std::stoi(weightField->value()) == 0) {
                continue; // Skip if either is missing or weight is 0
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
            // Swap chance field too
            ArrayField<NumericField<float>>* arrField = new ArrayField<NumericField<float>>(chanceField->name(), chanceField->value(), false, 0.0f, 100.0f);
            _fieldMap[chanceField->name()] = arrField;
            delete chanceField;
            chanceField = nullptr;
            // Add lootable
            _lootables[slot] = Lootable(loots, arrField);
        }
    }

    _isParsed = true;
}

void Monster::updateFromChild() {
    parse();
    for (auto& [key, l] : _lootables) {
        l.updateFromChild();
    }
    // Chances are updated in applyChanges
}

void Monster::adjustLootAmount(float multiplier) {
    parse();
    _modifiedLootAmount *= multiplier;
    for (auto&[key, l] : _lootables) {
        l.adjustLootAmount(multiplier);
    }
}

void Monster::adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types/* = std::vector<ItemType>()*/,
    std::vector<ItemClass> rarities/* = std::vector<ItemClass>()*/, bool isAnd/* = false*/) {
    parse();

    for (auto&[key, l] : _lootables) {
        l.adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
    }
}