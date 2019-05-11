#include "ItemBase.h"

void ItemBase::parse() {
    if (_isParsed) {
        return;
    }

    DBRBase::parse();
    _itemType = DBRBase::ItemTypeOf(_templateName);

    if (_fieldMap["cannotPickUpMultiple"] != nullptr) {
        _cannotPickupMultiple = _fieldMap["cannotPickUpMultiple"]->value() == "1";
    }

    if (_fieldMap["itemClassification"] != nullptr) {
        _itemClass = DBRBase::ItemClassOf(_fieldMap["itemClassification"]->value());
    }

    if (_fieldMap["maxStackSize"] != nullptr) {
        Field* field = _fieldMap["maxStackSize"];
        _maxStackSize = new NumericField<int>(field->name(), field->value());
        _fieldMap["maxStackSize"] = _maxStackSize;
        delete field;
        field = nullptr;
    }

    _isParsed = true;
}

void ItemBase::setMaxStackSize(int size) {
    if(_maxStackSize != nullptr) _maxStackSize->setModifiedValue((float)size);
}