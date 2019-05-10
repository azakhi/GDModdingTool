#pragma once

#include "DBRBase.h"

class ItemBase : public DBRBase
{
    bool _isParsed = false;
    bool _cannotPickupMultiple = false;
    ItemClass _itemClass = ItemClass::ClassNone;
    ItemType _itemType = ItemType::TypeNone;
    NumericField<int>* _maxStackSize = nullptr;

public:
    ItemBase(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName)
        : DBRBase(fileManager, directoryEntry, templateName) {}

    const bool cannotPickupMultiple() const { return _cannotPickupMultiple; }
    const ItemClass itemClass() const { return _itemClass; }
    const ItemType itemType() const { return _itemType; }

    void parse();
    void setMaxStackSize(int size);
};