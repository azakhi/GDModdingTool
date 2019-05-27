#include "FileManager.h"

#define NOMINMAX

#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <thread>
#include <Windows.h>

#include "DBRFiles/DBRBase.h"
#include "DBRFiles/FixedItemLoot.h"
#include "DBRFiles/Monster.h"
#include "DBRFiles/LootMasterTable.h"
#include "DBRFiles/ProxyPool.h"
#include "DBRFiles/ItemBase.h"
#include "DBRFiles/DynWeightAffixTable.h"
#include "DBRFiles/LevelTable.h"
#include "DBRFiles/ExperienceLevelControl.h"

#include <iostream>

FileManager::FileManager(std::string recordsDirectory, std::string addRecordsDirectory, std::string modDirectory, std::vector<std::string> subDirectories) {
    if (!std::filesystem::exists(recordsDirectory)) {
        throw std::runtime_error("Error: Couldn't find records directory: " + recordsDirectory);
    }

    if (!std::filesystem::exists(modDirectory)) {
        throw std::runtime_error("Error: Couldn't find mod directory: " + modDirectory);
    }

    if (addRecordsDirectory != "" && !std::filesystem::exists(addRecordsDirectory)) {
        throw std::runtime_error("Error: Couldn't find additional records directory: " + addRecordsDirectory);
    }

    _progressTotal = 0;
    _threadProgress.resize(THREAD_COUNT, 0);
    _threadStatus.resize(THREAD_COUNT, ThreadStatus::NotRunning);
    _recordsDirectory = recordsDirectory;
    _addRecordsDirectory = addRecordsDirectory;
    _modDirectory = modDirectory;
    _subDirectories = subDirectories;

    auto t = std::thread(&FileManager::_scanFiles, this);
    t.detach();
}

const std::vector<std::string> FileManager::getTemplateNames() const {
    return _templateNames;
}

DBRBase* FileManager::getFile(std::string path) const {
    auto it = _fileMap.find(path);
    if (it != _fileMap.end()) {
        return it->second;
    }

    return nullptr;
}

std::vector<DBRBase*> FileManager::getFiles(std::string templateName) const {
    auto it = _templateMap.find(templateName);
    if (it != _templateMap.end()) {
        return it->second->dbrs;
    }

    return std::vector<DBRBase*>();
}

std::vector<DBRBase*> FileManager::getFiles(std::vector<std::string> templateNames) const {
    std::vector<DBRBase*> files;

    for (const auto& tname : templateNames) {
        auto it = _templateMap.find(tname);
        if (it != _templateMap.end()) {
            files.insert(files.end(), it->second->dbrs.begin(), it->second->dbrs.end());
        }
    }


    return files;
}

std::vector<DBRBase*> FileManager::getFiles(std::vector<std::type_index> typeIndexes) const {
    std::vector<DBRBase*> files;

    for (const auto& tindex : typeIndexes) {
        auto it = _typeMap.find(tindex);
        if (it != _typeMap.end()) {
            files.insert(files.end(), it->second.begin(), it->second.end());
        }
    }


    return files;
}

void FileManager::_scanFiles() {
    try {
        _threadProgress[0] = 0;
        _threadStatus[0] = ThreadStatus::Running;

        std::unordered_map<std::string, std::function<void(std::filesystem::directory_entry, std::string, int)>> templateMap;
        templateMap["fixeditemloot.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<FixedItemLoot>(de, s, po); };
        templateMap["monster.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<Monster>(de, s, po); };
        templateMap["lootmastertable.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<LootMasterTable>(de, s, po); };
        templateMap["proxypool.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ProxyPool>(de, s, po); };
        templateMap["lootitemtable_dynweighted_dynaffix.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<DynWeightAffixTable>(de, s, po); };
        templateMap["leveltable.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<LevelTable>(de, s, po); };
        templateMap["experiencelevelcontrol.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ExperienceLevelControl>(de, s, po); };
        // Item Templates
        templateMap["itemfactionwarrant.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["jewelry_amulet.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemrandomsetformula.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemartifactformula.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemattributereset.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemsetformula.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemrelic.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemdifficultyunlock.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemdevotionreset.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemusableskill.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemnote.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["jewelry_medal.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["oneshot_potionhealth.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["oneshot_potionmana.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["oneshot_food.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["oneshot_scroll.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["questitem.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["itemartifact.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["jewelry_ring.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weaponarmor_shield.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_waist.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_vestment.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_shoulders.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_legs.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_hands.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_feet.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_clothing.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_chest.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["armor_head.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_sword2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_sword.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_staff.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_spear.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_scepter.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_ranged2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_ranged1h.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_offhand.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_mace2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_mace.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_dagger.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_axe2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };
        templateMap["weapon_axe.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po) { addTemplate<ItemBase>(de, s, po); };

        for (auto dir : _subDirectories) {
            // Read additional first to prevent creation of overriden files
            if (_addRecordsDirectory != "") {
                for (const auto & entry : std::filesystem::recursive_directory_iterator(_addRecordsDirectory + dir)) {
                    _threadProgress[0]++;
                    std::string templateName = _parseTemplateName(entry);
                    if (templateName != "") {
                        auto tempIt = templateMap.find(templateName);
                        if (tempIt != templateMap.end()) tempIt->second(entry, templateName, _addRecordsDirectory.length());
                        else addTemplate<DBRBase>(entry, templateName, _addRecordsDirectory.length());
                    }
                }
            }

            for (const auto & entry : std::filesystem::recursive_directory_iterator(_recordsDirectory + dir)) {
                _threadProgress[0]++;
                std::string templateName = _parseTemplateName(entry);
                if (templateName != "") {
                    auto tempIt = templateMap.find(templateName);
                    if (tempIt != templateMap.end()) tempIt->second(entry, templateName, _recordsDirectory.length());
                    else addTemplate<DBRBase>(entry, templateName, _recordsDirectory.length());
                }
            }
        }

        _threadStatus[0] = ThreadStatus::Completed;
    }
    catch (const std::exception& e) {
        log_warning << "Error while scanning files\n";
        log_error << e.what() << "\n";
        _threadStatus[0] = ThreadStatus::ThrownError;
    }
    catch (const char* errorMessage) {
        log_warning << "Error while scanning files\n";
        log_error << errorMessage << "\n";
        _threadStatus[0] = ThreadStatus::ThrownError;
    }
}

void FileManager::modifyField(std::string templateName, std::string fieldName, std::function<std::string(std::string)> modifier) {
    if (_templateMap.find(templateName) == _templateMap.end()) {
        return;
    }

    for (auto temp : _templateMap[templateName]->dbrs) {
        temp->modifyField(fieldName, modifier);
    }
}

void FileManager::modifyField(std::string templateName, std::vector<std::string> fieldNames, std::function<std::string(std::string)> modifier) {
    if (_templateMap.find(templateName) == _templateMap.end()) {
        return;
    }

    for (auto temp : _templateMap[templateName]->dbrs) {
        temp->modifyField(fieldNames, modifier);
    }
}

template <typename T>
void FileManager::addTemplate(std::filesystem::directory_entry directoryEntry, std::string templateName, int pathOffset) {
    if (!std::is_base_of<DBRBase, T>::value) {
        throw std::runtime_error("Unknow type T in File Manager");
    }

    std::string pathAsValue = "records\\" + directoryEntry.path().string().substr(pathOffset);
    std::replace(pathAsValue.begin(), pathAsValue.end(), '\\', '/');
    if (_fileMap.find(pathAsValue) != _fileMap.end()) {
        log_info << "File overriden: " << pathAsValue << "\n";
        return;
    }

    DBRBase* temp = (DBRBase*)(new T(this, directoryEntry, templateName));
    std::type_index typeIndex(typeid(T));
    if (_typeMap.find(typeIndex) == _typeMap.end()) _typeMap[typeIndex] = std::vector<DBRBase*>();
    _typeMap[typeIndex].push_back(temp);
    _templateMap[templateName]->dbrs.push_back(temp);
    _fileMap[pathAsValue] = temp;
}

void FileManager::save() {
    std::vector<std::pair<DBRBase*, std::string>> dirtyFiles;
    for (auto& it : _fileMap) {
        if (it.second->isDirty()) {
            dirtyFiles.push_back(std::make_pair(it.second, it.first));
        }
    }

    _progressTotal = (int)dirtyFiles.size();
    int size = (int)(dirtyFiles.size() / _threadProgress.size() + 1);
    for (size_t i = 0; i < _threadProgress.size(); i++) {
        _threadStatus[i] = ThreadStatus::Running;
        auto t = std::thread(&FileManager::_save, this, (int)i, size, dirtyFiles);
        t.detach();
    }
}

void FileManager::_save(int tnum, int size, std::vector<std::pair<DBRBase*, std::string>> temps) {
    try {
        int end = (tnum + 1) * size;
        if ((int)(temps.size()) < end) {
            end = (int)(temps.size());
        }

        _threadStatus[tnum] = ThreadStatus::Running;
        _threadProgress[tnum] = 0;
        for (int i = (tnum * size); i < end; i++) {
            auto temp = temps[i].first;
            if (temp->isDirty()) {
                std::string filePath = _modDirectory + "database\\" + temps[i].second;
                std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
                std::ofstream dbrFile;
                dbrFile.open(filePath);
                if (dbrFile.is_open()) {
                    dbrFile << temp->text();
                    dbrFile.flush();
                    dbrFile.close();
                }
                else {
                    std::cout << "File is not open\n" << GetLastError() << "\n";
                }

                _threadProgress[tnum]++;
            }
        }
        _threadStatus[tnum] = ThreadStatus::Completed;
    }
    catch (const std::exception& e) {
        log_warning << "Error while saving in thread " << tnum << "\n";
        log_error << e.what() << "\n";
        _threadStatus[tnum] = ThreadStatus::ThrownError;
    }
    catch (const char* errorMessage) {
        log_warning << "Error while saving in thread " << tnum << "\n";
        log_error << errorMessage << "\n";
        _threadStatus[tnum] = ThreadStatus::ThrownError;
    }
}

size_t FileManager::_addField(DBRData* data, std::string field) {
    std::lock_guard<std::mutex> lock(data->lock);
    auto it = data->fieldMap.find(field);
    if (it != data->fieldMap.end()) {
        return it->second;
    }

    size_t index = data->fieldMap.size();
    data->fieldMap[field] = index;
    return index;
}

std::string FileManager::_parseTemplateName(std::filesystem::directory_entry entry) {
    if (entry.path().extension().string() != ".dbr") {
        return "";
    }

    std::string templateName = "";
    std::ifstream dbrFile;
    dbrFile.open(entry.path());
    std::string line;
    while (std::getline(dbrFile, line)) {
        if (templateName == "" && line.substr(0, 13) == "templateName,") {
            templateName = line.substr(32, line.length() - 33);
            break;
        }
    }

    dbrFile.close();

    if (templateName != "" && _templateMap.find(templateName) == _templateMap.end()) {
        DBRData* data = new DBRData();
        _templateMap[templateName] = data;
        _templateNames.push_back(templateName);
    }

    return templateName;
}