#pragma once

#include <string>
#include <vector>

#include "DBRBase.h"
#include "Field.h"

class Lootable {
    float _modifiedLootAmount = 1.0f;
    ArrayField<NumericField<float>>* _chance;
    std::vector<Loot> _loots;

    void _updateChances();
    bool _isRatioPreserved() const;
public:
    Lootable(std::vector<Loot> loots = std::vector<Loot>(), ArrayField<NumericField<float>>* chance = nullptr);

    int totalWeight() const {
        int total = 0;
        for (const auto& l : _loots) total += l.weight->numericValue();
        return total;
    }

    int totalModifiedWeight() const {
        int total = 0;
        for (const auto& l : _loots) total += (int)(l.weight->numericModifiedValue());
        return total;
    }

    bool isDirty() const {
        return !_isRatioPreserved();
    }

    const ArrayField<NumericField<float>>* chance() const {
        return _chance;
    }

    void updateFromChild();
    void adjustLootAmount(float multiplier);
    void adjustSpecificLootAmount(float multiplier, std::vector<ItemType> types = std::vector<ItemType>(), std::vector<ItemClass> rarities = std::vector<ItemClass>(), bool isAnd = false);
};