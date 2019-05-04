#include "DynWeightAffixTable.h"

void DynWeightAffixTable::parse() {
    if (_isParsed) {
        return;
    }

    LootMasterTable::parse();
    {
        auto it = _fields.find("noPrefixNoSuffix");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _noPrefixNoSuffix = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _noPrefixNoSuffix;
            _fields[field->name()] = std::make_pair(it->second.first, _noPrefixNoSuffix);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("prefixOnly");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _prefixOnly = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _prefixOnly;
            _fields[field->name()] = std::make_pair(it->second.first, _prefixOnly);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("suffixOnly");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _suffixOnly = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _suffixOnly;
            _fields[field->name()] = std::make_pair(it->second.first, _suffixOnly);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("rarePrefixOnly");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _rarePrefixOnly = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _rarePrefixOnly;
            _fields[field->name()] = std::make_pair(it->second.first, _rarePrefixOnly);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("rareSuffixOnly");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _rareSuffixOnly = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _rareSuffixOnly;
            _fields[field->name()] = std::make_pair(it->second.first, _rareSuffixOnly);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("bothPrefixSuffix");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _bothPrefixSuffix = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _bothPrefixSuffix;
            _fields[field->name()] = std::make_pair(it->second.first, _bothPrefixSuffix);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("rareBothPrefixSuffix");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _rareBothPrefixSuffix = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _rareBothPrefixSuffix;
            _fields[field->name()] = std::make_pair(it->second.first, _rareBothPrefixSuffix);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("normalPrefixRareSuffix");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _normalPrefixRareSuffix = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _normalPrefixRareSuffix;
            _fields[field->name()] = std::make_pair(it->second.first, _normalPrefixRareSuffix);
            delete field;
            field = nullptr;
        }
    }
    {
        auto it = _fields.find("rarePrefixNormalSuffix");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _rarePrefixNormalSuffix = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _rarePrefixNormalSuffix;
            _fields[field->name()] = std::make_pair(it->second.first, _rarePrefixNormalSuffix);
            delete field;
            field = nullptr;
        }
    }

    _isParsed = true;
}