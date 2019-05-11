#include "DynWeightAffixTable.h"

void DynWeightAffixTable::parse() {
    if (_isParsed) {
        return;
    }

    LootMasterTable::parse();

    if (_fieldMap["noPrefixNoSuffix"] != nullptr) {
        Field* field = _fieldMap["noPrefixNoSuffix"];
        _noPrefixNoSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["noPrefixNoSuffix"] = _noPrefixNoSuffix;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["prefixOnly"] != nullptr) {
        Field* field = _fieldMap["prefixOnly"];
        _prefixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["prefixOnly"] = _prefixOnly;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["suffixOnly"] != nullptr) {
        Field* field = _fieldMap["suffixOnly"];
        _suffixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["suffixOnly"] = _suffixOnly;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rarePrefixOnly"] != nullptr) {
        Field* field = _fieldMap["rarePrefixOnly"];
        _rarePrefixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["rarePrefixOnly"] = _rarePrefixOnly;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rareSuffixOnly"] != nullptr) {
        Field* field = _fieldMap["rareSuffixOnly"];
        _rareSuffixOnly = new NumericField<int>(field->name(), field->value());
        _fieldMap["rareSuffixOnly"] = _rareSuffixOnly;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["bothPrefixSuffix"] != nullptr) {
        Field* field = _fieldMap["bothPrefixSuffix"];
        _bothPrefixSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["bothPrefixSuffix"] = _bothPrefixSuffix;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rareBothPrefixSuffix"] != nullptr) {
        Field* field = _fieldMap["rareBothPrefixSuffix"];
        _rareBothPrefixSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["rareBothPrefixSuffix"] = _rareBothPrefixSuffix;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["normalPrefixRareSuffix"] != nullptr) {
        Field* field = _fieldMap["normalPrefixRareSuffix"];
        _normalPrefixRareSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["normalPrefixRareSuffix"] = _normalPrefixRareSuffix;
        delete field;
        field = nullptr;
    }

    if (_fieldMap["rarePrefixNormalSuffix"] != nullptr) {
        Field* field = _fieldMap["rarePrefixNormalSuffix"];
        _rarePrefixNormalSuffix = new NumericField<int>(field->name(), field->value());
        _fieldMap["rarePrefixNormalSuffix"] = _rarePrefixNormalSuffix;
        delete field;
        field = nullptr;
    }

    _isParsed = true;
}