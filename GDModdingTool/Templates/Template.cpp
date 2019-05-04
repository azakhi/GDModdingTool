#include "Template.h"

#include <iostream>

Template::Template(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName) : _fileManager(fileManager) {
    _directoryEntry = directoryEntry;
    _templateName = templateName;
    _isParsed = false;
}

void Template::parse(bool isFullyParse = false, std::vector<std::string> fields = std::vector<std::string>()) {
    if (_isParsed) {
        return;
    }

    std::ifstream dbrFile;
    dbrFile.open(_directoryEntry.path());
    std::string line;
    while (std::getline(dbrFile, line)) {
        std::size_t commaPos = -1;
        commaPos = line.find_first_of(',');
        if (commaPos >= 0) {
            Field* field = new Field(line.substr(0, commaPos), line.substr(commaPos + 1, line.length() - commaPos - 2));
            _fields[field->name()] = std::make_pair(_fieldsOrdered.size(), field);
            _fieldsOrdered.push_back(field);
        }
        else {
            _fieldsOrdered.push_back(nullptr);
        }
    }

    _isParsed = true;
}

void Template::applyChanges() {
    // Should be overriden in case of delayed changes
}

void Template::addFieldIfNotExists(std::string fieldName, std::string value) {
    parse();
    if (_fields.find(fieldName) != _fields.end()) {
        // exists, return
        return;
    }

    Field* field = new Field(fieldName, "");
    field->setModifiedValue(value); // To make it 'modified'
    _fields[fieldName] = std::make_pair(_fieldsOrdered.size(), field);
    _fieldsOrdered.push_back(field);
}

void Template::modifyField(std::string fieldName, std::function<std::string(std::string)> modifier) {
    parse();
    auto it = _fields.find(fieldName);
    if (it != _fields.end()) {
        it->second.second->setModifiedValue(modifier(it->second.second->value()));
    }
}

void Template::modifyField(std::vector<std::string> fieldNames, std::function<std::string(std::string)> modifier) {
    for (auto& fieldName : fieldNames) {
        modifyField(fieldName, modifier);
    }
}

std::string Template::text() {
    parse();
    applyChanges();

    std::string str = "";
    for (auto f : _fieldsOrdered) {
        str += f->name() + "," + f->toString() + ",\n";
    }

    std::ifstream dbrFile;
    dbrFile.open(_directoryEntry.path());
    std::string line;
    size_t lineNum = 0;
    while (std::getline(dbrFile, line)) {
        if (_fieldsOrdered[lineNum] == nullptr) {
            str += line + "\n";
        }
        else {
            str += _fieldsOrdered[lineNum]->name() + "," + _fieldsOrdered[lineNum]->toString() + ",\n";
        }
        lineNum++;
    }

    for (size_t i = lineNum; i < _fieldsOrdered.size(); i++) {
        if (_fieldsOrdered[i] == nullptr) {
            str += _fieldsOrdered[i]->name() + "," + _fieldsOrdered[i]->toString() + ",\n";
        }
    }

    return str;
}