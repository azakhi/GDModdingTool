#include "Lootable.h"

#include "LootMasterTable.h"
#include "LevelTable.h"
#include "DynWeightAffixTable.h"
#include "ItemBase.h"

Lootable::Lootable(std::vector<Loot> loots/* = std::vector<Loot>()*/, ArrayField<NumericField<float>>* chance/* = nullptr*/) {
    _chance = chance;

    for (int i = (int)loots.size() - 1; i >= 0; i--) {
        if (loots[i].name == nullptr || loots[i].weight == nullptr || loots[i].weight->numericValue() == 0) {
            loots.erase(loots.begin() + i);
        }
    }

    _loots = loots;
}

void Lootable::updateFromChild() {
    for (auto& l : _loots) {
        if (l.file == nullptr) continue;
        if (dynamic_cast<LootMasterTable*>(l.file)) {
            const LootMasterTable* lmt = (LootMasterTable*)l.file;
            if (lmt->totalWeight() > 0) {
                float mult = (float)(lmt->totalModifiedWeight()) / lmt->totalWeight();
                l.weight->setModifiedValue(l.weight->numericValue() * mult);
            }
        }
        else if (dynamic_cast<LevelTable*>(l.file)) {
            float totWeight = 0.0f;
            float totModWeight = 0.0f;
            std::vector<DynWeightAffixTable*> records = ((LevelTable*)l.file)->records();
            for (auto r : records) {
                totWeight += (float)(r->totalWeight());
                totModWeight += (float)(r->totalModifiedWeight());
            }

            if (totWeight > 0) {
                l.weight->setModifiedValue(l.weight->numericValue() * (totModWeight / totWeight));
            }
        }
    }

    _updateChances();
}

void Lootable::adjustLootAmount(float multiplier) {
    _modifiedLootAmount = multiplier;
    _updateChances();
}

void Lootable::adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types/* = std::vector<ItemType>()*/,
    std::vector<ItemClass> rarities/* = std::vector<ItemClass>()*/, bool isAnd/* = false*/) {
    for (auto& l : _loots) {
        if (l.file == nullptr) continue;
        if (dynamic_cast<LootMasterTable*>(l.file)) {}
        else if (dynamic_cast<LevelTable*>(l.file)) {}
        else if (dynamic_cast<ItemBase*>(l.file))
        {
            ItemBase* item = (ItemBase*)l.file;
            if (item->cannotPickupMultiple()) continue;
            bool isTypeMatch = std::find(types.begin(), types.end(), item->itemType()) != types.end();
            bool isRarityMatch = std::find(rarities.begin(), rarities.end(), item->itemClass()) != rarities.end();

            if ((isAnd && isTypeMatch && isRarityMatch) || (!isAnd && (isRarityMatch || isTypeMatch))) {
                l.weight->setModifiedValue(l.weight->numericValue() * multiplier);
            }
        }
        else {
            throw "Unknown file type in Lootable";
        }
    }

    _updateChances();
}

void Lootable::_updateChances() {
    if (_chance == nullptr) return;

    int totWeight = totalWeight();
    float totModWeight = (float)totalModifiedWeight();

    float chanceMultiplier = _modifiedLootAmount;
    if (totWeight > 0) {
        chanceMultiplier *= totModWeight / totWeight;
    }

    std::vector<NumericField<float>>& chances = _chance->values();
    for (auto& c : chances) {
        c.setModifiedValue(c.numericValue() * chanceMultiplier);
    }
}

bool Lootable::_isRatioPreserved() const {
    if (_loots.size() < 2) {
        return true;
    }

    float ratio = -1;

    for (const auto& l : _loots) {
        int modifiedWeight = l.weight->numericModifiedValue();
        if (modifiedWeight == 0) {
            if (l.weight->numericValue() == 0) {
                continue;
            }
            else {
                return false;
            }
        }

        float r = l.weight->numericValue() / (float)modifiedWeight;
        if (ratio < 0) {
            ratio = r;
        }
        else if (ratio != r) {
            return false;
        }
    }

    return true;
}