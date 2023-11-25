#include "DBRBase.h"

void DBRBase::adjustFormulaField(std::string fieldName, float multiplier) {
    if (_fieldMap[fieldName] != nullptr) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(2) << "(" << _fieldMap[fieldName]->value() << ") * " << multiplier;
        _fieldMap[fieldName]->setModifiedValue(os.str());
    }
}

void DBRBase::setFormulaField(std::string fieldName, std::string value) {
    if (_fieldMap[fieldName] != nullptr) {
        _fieldMap[fieldName]->setModifiedValue(value);
    }
}

void DBRBase::parse() {
    if (_isParsed) {
        return;
    }

    std::ifstream dbrFile;
    dbrFile.open(_directoryEntry.path(), std::ios::binary);
    std::string line;
    while (std::getline(dbrFile, line)) {
        std::size_t commaPos = -1;
        commaPos = line.find_first_of(',');
        Field* field = nullptr;
        if (commaPos >= 0) {
            field = new Field(line.substr(0, commaPos), line.substr(commaPos + 1, line.length() - commaPos - 2));
        }

        _fieldMap[field->name()] = field;
    }

    _isParsed = true;
}

void DBRBase::applyChanges() {
    // Should be overriden in case of delayed changes
}

void DBRBase::addFieldIfNotExists(std::string fieldName, std::string value) {
    parse();
    if (_fieldMap[fieldName] != nullptr) {
        // exists, return
        return;
    }

    Field* field = new Field(fieldName, "");
    field->setModifiedValue(value); // To make it 'modified'
    _fieldMap[fieldName] = field;
}

void DBRBase::modifyField(std::string fieldName, std::function<std::string(std::string)> modifier) {
    parse();
    if (_fieldMap[fieldName] != nullptr) {
        _fieldMap[fieldName]->setModifiedValue(modifier(_fieldMap[fieldName]->value()));
    }
}

void DBRBase::modifyField(std::vector<std::string> fieldNames, std::function<std::string(std::string)> modifier) {
    for (auto& fieldName : fieldNames) {
        modifyField(fieldName, modifier);
    }
}

std::string DBRBase::text() {
    parse();
    applyChanges();

    std::string str = "";
    for (auto f : _fieldMap.fieldsOrdered()) {
        if (f != nullptr) {
            str += f->name() + "," + f->toString() + ",\n";
        }
        else {
            //str += "\n";
        }
    }

    return str;
}