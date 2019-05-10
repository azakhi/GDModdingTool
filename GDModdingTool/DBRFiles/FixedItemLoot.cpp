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
        auto chanceIt = _fields.find("loot" + std::to_string(lootNum) + "Chance");
        if (chanceIt == _fields.end()) {
            continue; // No need to add
        }

        Field* chanceField = chanceIt->second.second;
        ArrayField<NumericField<float>>* arrField = new ArrayField<NumericField<float>>(chanceField->name(), chanceField->value());
        if (arrField->totalValue() < FLT_MIN) { // No need to add
            delete arrField;
            arrField = nullptr;
            continue;
        }

        // Find name-weight matches
        std::vector<Loot> loots;
        for (int i = 1; i <= 6; i++) {
            auto nameIt = _fields.find("loot" + std::to_string(lootNum) + "Name" + std::to_string(i));
            auto weightIt = _fields.find("loot" + std::to_string(lootNum) + "Weight" + std::to_string(i));
            if (nameIt == _fields.end() || weightIt == _fields.end() || std::stoi(weightIt->second.second->value()) == 0) {
                continue; // Skip if either is missing or weight is 0
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
            // Swap chance field too
            _fieldsOrdered[chanceIt->second.first] = arrField;
            _fields[chanceField->name()] = std::make_pair(chanceIt->second.first, arrField);
            delete chanceField;
            chanceField = nullptr;
            // Add lootable
            _lootables.push_back(Lootable(loots, arrField));
        }
    }

/*
    std::vector<int> lootCounts;
    std::vector<std::vector<Loot>> lootsVec;
    std::vector<ArrayField<NumericField<float>>*> chanceVec;
    for (size_t i = 0; i < _fieldsOrdered.size(); i++) {
        Field* field = _fieldsOrdered[i];
        std::string name = field->name();

        if (name.substr(0, 4) == "loot") {
            size_t lootNum = std::stoi(name.substr(4, 1)) - 1;

            if (lootNum >= _lootables.size()) {
                lootsVec.insert(lootsVec.end(), lootNum - lootsVec.size() + 1, std::vector<Loot>());
                chanceVec.insert(chanceVec.end(), lootNum - chanceVec.size() + 1, nullptr);
                lootCounts.insert(lootCounts.end(), lootNum - lootCounts.size() + 1, 0);
            }

            if (name.substr(5, 6) == "Chance") {
                ArrayField<NumericField<float>>* arrField = new ArrayField<NumericField<float>>(name, field->value());
                chanceVec[lootNum] = arrField;
                _fieldsOrdered[i] = arrField;
                _fields[name] = arrField;
                delete field;
                field = nullptr;
            }
            else if (name.substr(5, 4) == "Name") {
                lootCounts[lootNum]++;
                size_t num = std::stoi(name.substr(9, 1)) - 1;
                if (num >= lootsVec[lootNum].size()) {
                    lootsVec[lootNum].insert(lootsVec[lootNum].end(), num - lootsVec[lootNum].size() + 1, Loot());
                }

                lootsVec[lootNum][num].file = _fileManager->getFile(field->value());
                lootsVec[lootNum][num].type = DBRBase::TypeOf(lootsVec[lootNum][num].file);
                lootsVec[lootNum][num].rarity = DBRBase::RarityOf(lootsVec[lootNum][num].file);
                lootsVec[lootNum][num].name = field;
            }
            else if (name.substr(5, 6) == "Weight") {
                size_t num = std::stoi(name.substr(11, 1)) - 1;
                if (num >= lootsVec[lootNum].size()) {
                    lootsVec[lootNum].insert(lootsVec[lootNum].end(), num - lootsVec[lootNum].size() + 1, Loot());
                }

                NumericField<int>* weight = new NumericField<int>(name, field->value());
                lootsVec[lootNum][num].weight = weight;
                _fieldsOrdered[i] = weight;
                _fields[name] = weight;
                delete field;
                field = nullptr;
            }

        }
    }

    // Remove unnecessary loots
    for (size_t i = 0; i < lootsVec.size(); i++) {
        lootsVec[i].erase(lootsVec[i].begin() + lootCounts[i], lootsVec[i].end());
    }

    for (size_t i = 0; i < lootsVec.size(); i++) {
        if (chanceVec[i] != nullptr && chanceVec[i]->totalValue() > 0.0f) {
            _lootables.push_back(Lootable(lootsVec[i], chanceVec[i]));
        }
    }
*/

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

    auto maxEq = _fields.find("numSpawnMaxEquation");
    auto minEq = _fields.find("numSpawnMinEquation");
    if (maxEq != _fields.end()) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(1) << "(" << maxEq->second.second->value() << ") * " << mult;
        maxEq->second.second->setModifiedValue(os.str());
    }
    if (minEq != _fields.end()) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(1) << "(" << minEq->second.second->value() << ") * " << mult;
        minEq->second.second->setModifiedValue(os.str());
    }
}