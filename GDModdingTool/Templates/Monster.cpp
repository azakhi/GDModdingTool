#include "Monster.h"

#include <regex>

void Monster::parse() {
    if (_isParsed) {
        return;
    }

    Template::parse();
    _modifiedLootAmount = 1.0f;

    {
        auto it = _fields.find("monsterClassification");
        if (it != _fields.end()) {
            _monsterClass = Template::MonsterClassOf(it->second.second->value());
        }
    }
    {
        auto it = _fields.find("factions");
        if (it != _fields.end()) {
            _faction = _fileManager->getFaction(it->second.second->value());
        }
    }

    std::vector<std::string> itemSlots({ "Head", "Chest", "Shoulders", "Hands", "Legs", "Feet", "RightHand", "LeftHand", "Finger1", "Finger2", "Misc1", "Misc2", "Misc3" });
    for (const auto& slot : itemSlots) {
        // Check for chance first
        auto chanceIt = _fields.find("chanceToEquip" + slot);
        if (chanceIt == _fields.end() || std::stof(chanceIt->second.second->value()) < FLT_MIN) {
            continue; // No need to add
        }

        // Find name-weight matches
        std::vector<Loot> loots;
        for (int i = 1; i <= 6; i++) {
            auto nameIt = _fields.find("loot" + slot + "Item" + std::to_string(i));
            auto weightIt = _fields.find("chanceToEquip" + slot + "Item" + std::to_string(i));
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
            Field* chanceField = chanceIt->second.second;
            ArrayField<NumericField<float>>* arrField = new ArrayField<NumericField<float>>(chanceField->name(), chanceField->value(), false, 0.0f, 100.0f);
            _fieldsOrdered[chanceIt->second.first] = arrField;
            _fields[chanceField->name()] = std::make_pair(chanceIt->second.first, arrField);
            delete chanceField;
            chanceField = nullptr;
            // Add lootable
            _lootables[slot] = Lootable(loots, arrField);
        }
    }

/*
    std::unordered_map<std::string, std::vector<Loot>> lootsMap;
    std::unordered_map<std::string, ArrayField<NumericField<float>>*> chanceMap;
    for (size_t i = 0; i < _fieldsOrdered.size(); i++) {
        Field* field = _fieldsOrdered[i];
        std::string name = field->name();
        if (name == "monsterClassification") {
            _monsterClass = Template::MonsterClassOf(field->value());
        }
        else if (name == "factions") {
            _faction = _fileManager->getFaction(field->value());
        }
        else if (std::regex_match(name, std::regex("loot[A-z]+[0-9]+Item[0-9]+"))) { //(v.name.substr(0, 4) == "loot" && v.name.substr(v.name.size() - 5, 4) == "Item") {
            std::string lootName = name.substr(4, name.size() - 5 - 4);
            size_t lootNum = std::stoi(name.substr(name.size() - 1)) - 1;

            std::vector<Loot>& loots = lootsMap[lootName];
            if (lootNum >= loots.size()) {
                loots.insert(loots.end(), lootNum - loots.size() + 1, Loot());
            }

            loots[lootNum].file = _fileManager->getFile(field->value());
            loots[lootNum].type = Template::TypeOf(loots[lootNum].file);
            loots[lootNum].rarity = Template::RarityOf(loots[lootNum].file);
            loots[lootNum].name = field;
        }
        else if (std::regex_match(name, std::regex("chanceToEquip[A-z]+[0-9]*Item[0-9]+"))) { //(name.substr(0, 13) == "chanceToEquip") {
            std::string lootName = name.substr(13, name.size() - 5 - 13);
            size_t lootNum = std::stoi(name.substr(name.size() - 1)) - 1;

            std::vector<Loot>& loots = lootsMap[lootName];
            if (lootNum >= loots.size()) {
                loots.insert(loots.end(), lootNum - loots.size() + 1, Loot());
            }

            NumericField<int>* weight = new NumericField<int>(name, field->value());
            loots[lootNum].weight = weight;
            _fieldsOrdered[i] = weight;
            _fields[name] = weight;
            delete field;
            field = nullptr;
        }
        else if (std::regex_match(name, std::regex("chanceToEquip[A-z]+[0-9]*"))) { //(name.substr(0, 13) == "chanceToEquip") {
            std::string lootName = name.substr(13);

            ArrayField<NumericField<float>>* arrField = new ArrayField<NumericField<float>>(name, field->value());
            chanceMap[lootName] = arrField;
            _fieldsOrdered[i] = arrField;
            _fields[name] = arrField;
            delete field;
            field = nullptr;
        }
    }

    for (const auto&[key, value] : lootsMap) {
        auto chanceIt = chanceMap.find(key);
        if (chanceIt != chanceMap.end() && chanceIt->second->totalValue() > 0.0f) {
            _lootables[key] = Lootable(value, chanceMap[key]);
        }
    }
*/

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