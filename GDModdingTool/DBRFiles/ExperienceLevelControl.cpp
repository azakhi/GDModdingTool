#include "ExperienceLevelControl.h"

void ExperienceLevelControl::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();

    if (_fieldMap["maxPlayerLevel"] != nullptr) {
        Field* field = _fieldMap["maxPlayerLevel"];
        _maxPlayerLevel = new NumericField<int>(field->name(), field->value());
        _fieldMap["maxPlayerLevel"] = _maxPlayerLevel;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["characterModifierPoints"] != nullptr) {
        Field* field = _fieldMap["characterModifierPoints"];
        _charModPoints = new NumericField<int>(field->name(), field->value());
        _fieldMap["characterModifierPoints"] = _charModPoints;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["maxDevotionPoints"] != nullptr) {
        Field* field = _fieldMap["maxDevotionPoints"];
        _maxDevPoints = new NumericField<int>(field->name(), field->value());
        _fieldMap["maxDevotionPoints"] = _maxDevPoints;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["dexterityIncrement"] != nullptr) {
        Field* field = _fieldMap["dexterityIncrement"];
        _dexInc = new NumericField<int>(field->name(), field->value());
        _fieldMap["dexterityIncrement"] = _dexInc;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["strengthIncrement"] != nullptr) {
        Field* field = _fieldMap["strengthIncrement"];
        _strInc = new NumericField<int>(field->name(), field->value());
        _fieldMap["strengthIncrement"] = _strInc;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["intelligenceIncrement"] != nullptr) {
        Field* field = _fieldMap["intelligenceIncrement"];
        _intInc = new NumericField<int>(field->name(), field->value());
        _fieldMap["intelligenceIncrement"] = _intInc;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["lifeIncrement"] != nullptr) {
        Field* field = _fieldMap["lifeIncrement"];
        _lifeInc = new NumericField<int>(field->name(), field->value());
        _fieldMap["lifeIncrement"] = _lifeInc;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["lifeIncrementDexterity"] != nullptr) {
        Field* field = _fieldMap["lifeIncrementDexterity"];
        _lifeIncDex = new NumericField<int>(field->name(), field->value());
        _fieldMap["lifeIncrementDexterity"] = _lifeIncDex;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["lifeIncrementIntelligence"] != nullptr) {
        Field* field = _fieldMap["lifeIncrementIntelligence"];
        _lifeIncInt = new NumericField<int>(field->name(), field->value());
        _fieldMap["lifeIncrementIntelligence"] = _lifeIncInt;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["manaIncrement"] != nullptr) {
        Field* field = _fieldMap["manaIncrement"];
        _manaInc = new NumericField<int>(field->name(), field->value());
        _fieldMap["manaIncrement"] = _manaInc;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["skillModifierPoints"] != nullptr) {
        Field* field = _fieldMap["skillModifierPoints"];
        _skillModPoints = new ArrayField<NumericField<int>>(field->name(), field->value());
        _fieldMap["skillModifierPoints"] = _skillModPoints;
        delete field;
        field = nullptr;
    }

    _isParsed = true;
}

void ExperienceLevelControl::setMaxPlayerLevel(int value) {
    if (_maxPlayerLevel != nullptr) _maxPlayerLevel->setModifiedValue(value);
}

void ExperienceLevelControl::setCharModPoints(int value) {
    if (_charModPoints != nullptr) _charModPoints->setModifiedValue(value);
}

void ExperienceLevelControl::setMaxDevPoints(int value) {
    if (_maxDevPoints != nullptr) _maxDevPoints->setModifiedValue(value);
}

void ExperienceLevelControl::setDexInc(int value) {
    if (_dexInc != nullptr) _dexInc->setModifiedValue(value);
}

void ExperienceLevelControl::setStrInc(int value) {
    if (_strInc != nullptr) _strInc->setModifiedValue(value);
}

void ExperienceLevelControl::setIntInc(int value) {
    if (_intInc != nullptr) _intInc->setModifiedValue(value);
}

void ExperienceLevelControl::setLifeInc(int value) {
    if (_lifeInc != nullptr) _lifeInc->setModifiedValue(value);
}

void ExperienceLevelControl::setLifeIncDex(int value) {
    if (_lifeIncDex != nullptr) _lifeIncDex->setModifiedValue(value);
}

void ExperienceLevelControl::setLifeIncInt(int value) {
    if (_lifeIncInt != nullptr) _lifeIncInt->setModifiedValue(value);
}

void ExperienceLevelControl::setManaInc(int value) {
    if (_manaInc != nullptr) _manaInc->setModifiedValue(value);
}

void ExperienceLevelControl::adjustSkillModPointPerLevel(int change) {
    if (_skillModPoints != nullptr) {
        std::vector<NumericField<int>>& points = _skillModPoints->values();
        for (auto& p : points) {
            p.setModifiedValue(p.numericValue() + change);
        }
    }
}

void ExperienceLevelControl::setSkillModPointPerLevel(std::string value, char separator/* = ';'*/) {
    if (_skillModPoints != nullptr) {
        _skillModPoints->setModifiedValue(value, separator);
    }
}

void ExperienceLevelControl::adjustExpRequirement(float multiplier) {
    if (_fieldMap["experienceLevelEquation"] != nullptr) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(2) << "(" << _fieldMap["experienceLevelEquation"]->value() << ") * " << multiplier;
        _fieldMap["experienceLevelEquation"]->setModifiedValue(os.str());
    }
}

void ExperienceLevelControl::setExpRequirementEquation(std::string value) {
    if (_fieldMap["experienceLevelEquation"] != nullptr) {
        _fieldMap["experienceLevelEquation"]->setModifiedValue(value);
    }
}