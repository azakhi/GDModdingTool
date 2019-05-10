#include "ItemBase.h"

void ItemBase::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();
    _itemType = DBRBase::ItemTypeOf(_templateName);
    {
        auto it = _fields.find("cannotPickUpMultiple");
        if (it != _fields.end()) {
            _cannotPickupMultiple = it->second.second->value() == "1";
        }
    }
    {
        auto it = _fields.find("itemClassification");
        if (it != _fields.end()) {
            _itemClass = DBRBase::ItemClassOf(it->second.second->value());
        }
    }
    {
        auto it = _fields.find("maxStackSize");
        if (it != _fields.end()) {
            Field* field = it->second.second;
            _maxStackSize = new NumericField<int>(field->name(), field->value());
            _fieldsOrdered[it->second.first] = _maxStackSize;
            _fields[field->name()] = std::make_pair(it->second.first, _maxStackSize);
            delete field;
            field = nullptr;
        }
    }

    _isParsed = true;
}

void ItemBase::setMaxStackSize(int size) {
    if(_maxStackSize != nullptr) _maxStackSize->setModifiedValue((float)size);
}