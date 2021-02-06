#pragma once

#include "DBRBase.h"
#include "LootMasterTable.h"

class DynWeightAffixTable : public LootMasterTable
{
    bool _isParsed = false;
    NumericField<int>* _weights[3][3];
    //NumericField<int>* _noPrefixNoSuffix = nullptr;
    //NumericField<int>* _prefixOnly = nullptr;
    //NumericField<int>* _suffixOnly = nullptr;
    //NumericField<int>* _rarePrefixOnly = nullptr;
    //NumericField<int>* _rareSuffixOnly = nullptr;
    //NumericField<int>* _bothPrefixSuffix = nullptr;
    //NumericField<int>* _rareBothPrefixSuffix = nullptr;
    //NumericField<int>* _normalPrefixRareSuffix = nullptr;
    //NumericField<int>* _rarePrefixNormalSuffix = nullptr;

public:
    DynWeightAffixTable(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName, bool isAlwaysDirty = false)
        : LootMasterTable(fileManager, directoryEntry, templateName, isAlwaysDirty) {}

    void parse();
    void adjustAffixWeight(float multiplier, AffixType prefixType, AffixType suffixType);
};