#include "DynWeightAffixTable.h"

void DynWeightAffixTable::parse() {
    if (_isParsed) {
        return;
    }

    LootMasterTable::parse();

    if (_fieldMap["noPrefixNoSuffix"] != nullptr) {
        Field* field = _fieldMap["noPrefixNoSuffix"];
        _weights[0][0] = new NumericField<int>(field->name(), field->value());
        //_noPrefixNoSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["noPrefixNoSuffix"] = _weights[0][0];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["prefixOnly"] != nullptr) {
        Field* field = _fieldMap["prefixOnly"];
        _weights[1][0] = new NumericField<int>(field->name(), field->value());
        //_prefixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["prefixOnly"] = _weights[1][0];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["suffixOnly"] != nullptr) {
        Field* field = _fieldMap["suffixOnly"];
        _weights[0][1] = new NumericField<int>(field->name(), field->value());
        //_suffixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["suffixOnly"] = _weights[0][1];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rarePrefixOnly"] != nullptr) {
        Field* field = _fieldMap["rarePrefixOnly"];
        _weights[2][0] = new NumericField<int>(field->name(), field->value());
        //_rarePrefixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["rarePrefixOnly"] = _weights[2][0];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rareSuffixOnly"] != nullptr) {
        Field* field = _fieldMap["rareSuffixOnly"];
        _weights[0][2] = new NumericField<int>(field->name(), field->value());
        //_rareSuffixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["rareSuffixOnly"] = _weights[0][2];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["bothPrefixSuffix"] != nullptr) {
        Field* field = _fieldMap["bothPrefixSuffix"];
        _weights[1][1] = new NumericField<int>(field->name(), field->value());
        //_bothPrefixSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["bothPrefixSuffix"] = _weights[1][1];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rareBothPrefixSuffix"] != nullptr) {
        Field* field = _fieldMap["rareBothPrefixSuffix"];
        _weights[2][2] = new NumericField<int>(field->name(), field->value());
        //_rareBothPrefixSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["rareBothPrefixSuffix"] = _weights[2][2];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["normalPrefixRareSuffix"] != nullptr) {
        Field* field = _fieldMap["normalPrefixRareSuffix"];
        _weights[1][2] = new NumericField<int>(field->name(), field->value());
        //_normalPrefixRareSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["normalPrefixRareSuffix"] = _weights[1][2];
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rarePrefixNormalSuffix"] != nullptr) {
        Field* field = _fieldMap["rarePrefixNormalSuffix"];
        _weights[2][1] = new NumericField<int>(field->name(), field->value());
        //_rarePrefixNormalSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["rarePrefixNormalSuffix"] = _weights[2][1];
        delete field;
        field = nullptr;
    }

    _isParsed = true;
}

void DynWeightAffixTable::adjustAffixWeight(float multiplier, AffixType prefixType, AffixType suffixType) {
    NumericField<int>* field = _weights[(int)prefixType][(int)suffixType];
    if (field != nullptr) field->setModifiedValue(field->numericValue() * multiplier);
}