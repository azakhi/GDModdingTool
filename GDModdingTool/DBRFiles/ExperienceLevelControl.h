#pragma once

#include "DBRBase.h"

class ExperienceLevelControl : public DBRBase {
    bool _isParsed = false;

    NumericField<int>* _maxPlayerLevel = nullptr;
    NumericField<int>* _charModPoints = nullptr;
    NumericField<int>* _maxDevPoints = nullptr;
    NumericField<int>* _dexInc = nullptr;
    NumericField<int>* _strInc = nullptr;
    NumericField<int>* _intInc = nullptr;
    NumericField<int>* _lifeInc = nullptr;
    NumericField<int>* _lifeIncDex = nullptr;
    NumericField<int>* _lifeIncInt = nullptr;
    NumericField<int>* _manaInc = nullptr;
    ArrayField<NumericField<int>>* _skillModPoints = nullptr;

public:
    ExperienceLevelControl(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName, bool isAlwaysDirty = false)
        : DBRBase(fileManager, directoryEntry, templateName, isAlwaysDirty) {}
    void parse();
    void setMaxPlayerLevel(int value);
    void setCharModPoints(int value);
    void setMaxDevPoints(int value);
    void setDexInc(int value);
    void setStrInc(int value);
    void setIntInc(int value);
    void setLifeInc(int value);
    void setLifeIncDex(int value);
    void setLifeIncInt(int value);
    void setManaInc(int value);
    void adjustSkillModPointPerLevel(int change);
    void setSkillModPointPerLevel(std::string value, char separator = ';');
    void adjustExpRequirement(float multiplier);
    void setExpRequirementEquation(std::string value);
};