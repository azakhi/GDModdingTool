#include "FixedItemLoot.h"

#include "LootMasterTable.h"

void FixedItemLoot::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();
    _modifiedLootAmount = 1.0f;

    for (int lootNum = 1; lootNum <= 6; lootNum++) {
        // Check for chance first
        Field* chanceField = _fieldMap["loot" + std::to_string(lootNum) + "Chance"];
        if (chanceField == nullptr) {
            continue; // No need to add
        }
        
        ArrayField<NumericField<float>>* arrField = new ArrayField<NumericField<float>>(chanceField->name(), chanceField->value());
        if (arrField->totalValue() < FLT_MIN) { // No need to add
            delete arrField;
            arrField = nullptr;
            continue;
        }

        // Find name-weight matches
        std::vector<Loot> loots;
        for (int i = 1; i <= 6; i++) {
            Field* nameField = _fieldMap["loot" + std::to_string(lootNum) + "Name" + std::to_string(i)];
            Field* weightField = _fieldMap["loot" + std::to_string(lootNum) + "Weight" + std::to_string(i)];
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
            _fieldMap[chanceField->name()] = arrField;
            delete chanceField;
            chanceField = nullptr;
            // Add lootable
            _lootables.push_back(Lootable(loots, arrField));
        }
    }

    _isParsed = true;
}

void FixedItemLoot::updateFromChild() {
    parse();
    for (auto& l : _lootables) l.updateFromChild();
    _updateLootEquations();
}

void FixedItemLoot::adjustLootAmount(float multiplier) {
    parse();
    _modifiedLootAmount = multiplier;
    for (auto& l : _lootables) l.updateFromChild();
    _updateLootEquations();
}

void FixedItemLoot::adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types/* = std::vector<ItemType>()*/,
    std::vector<ItemClass> rarities/* = std::vector<ItemClass>()*/, bool isAnd/* = false*/) {
    parse();
    for (auto&l : _lootables) {
        l.adjustSpecificLootAmount(multiplier, types, rarities, isAnd);
    }
}

void FixedItemLoot::_updateLootEquations() {
    float totalChance = 0.0f;
    float totalModifiedChance = 0.0f;
    for (auto& l : _lootables) {
        totalChance += l.chance()->totalValue();
        totalModifiedChance += l.chance()->totalModifiedValue();
    }

    float mult = _modifiedLootAmount;
    if (totalChance > 0.0f) mult *= totalModifiedChance / totalChance;

    if (std::abs(1.0f - mult) < 0.1f) return;

    if (_fieldMap["numSpawnMaxEquation"] != nullptr) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(1) << "(" << _fieldMap["numSpawnMaxEquation"]->value() << ") * " << mult;
        _fieldMap["numSpawnMaxEquation"]->setModifiedValue(os.str());
    }

    if (_fieldMap["numSpawnMinEquation"] != nullptr) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(1) << "(" << _fieldMap["numSpawnMinEquation"]->value() << ") * " << mult;
        _fieldMap["numSpawnMinEquation"]->setModifiedValue(os.str());
    }
}