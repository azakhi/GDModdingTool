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
#include "DBRFiles/ExperienceFormulas.h"

#include <iostream>

FileManager::FileManager(const Config& config) {
    if (!std::filesystem::exists(config.recordsDir)) {
        throw std::runtime_error("Error: Couldn't find records directory: " + config.recordsDir);
    }

    if (!std::filesystem::exists(config.modDir)) {
        throw std::runtime_error("Error: Couldn't find mod directory: " + config.modDir);
    }

    if (config.addRecordsDir != "" && !std::filesystem::exists(config.addRecordsDir)) {
        throw std::runtime_error("Error: Couldn't find additional records directory: " + config.addRecordsDir);
    }

    if (config.subDirs.size() < 1) {
        log_warning << "No subdirectory is given. All subdirectories will be scanned.\n";
    }

    if (config.addRecordsDir != "" && config.addSubDirs.size() < 1) {
        log_warning << "No subdirectory is given for additional source. All subdirectories will be scanned.\n";
    }

    _progressTotal = 0;
    _threadProgress.resize(THREAD_COUNT, 0);
    _threadStatus.resize(THREAD_COUNT, ThreadStatus::NotRunning);
    _recordsDirectory = config.recordsDir;
    _addRecordsDirectory = config.addRecordsDir;
    _modDirectory = config.modDir;
    _includedModDirs = config.includedModDirs;
    _subDirectories = config.subDirs;
    if (_subDirectories.size() < 1) {
        _subDirectories.push_back("");
    }

    _addSubDirectories = config.addSubDirs;
    if (_addSubDirectories.size() < 1) {
        _addSubDirectories.push_back("");
    }

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

        std::unordered_map<std::string, std::function<void(std::filesystem::directory_entry, std::string, int, bool)>> templateMap;
        templateMap["fixeditemloot.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<FixedItemLoot>(de, s, po, iad); };
        templateMap["monster.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<Monster>(de, s, po, iad); };
        templateMap["lootmastertable.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<LootMasterTable>(de, s, po, iad); };
        templateMap["proxypool.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<ProxyPool>(de, s, po, iad); };
        templateMap["lootitemtable_dynweighted_dynaffix.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<DynWeightAffixTable>(de, s, po, iad); };
        templateMap["leveltable.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<LevelTable>(de, s, po, iad); };
        templateMap["experiencelevelcontrol.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<ExperienceLevelControl>(de, s, po, iad); };
        templateMap["experienceformulas.tpl"] = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<ExperienceFormulas>(de, s, po, iad); };
        // Item Templates
        auto itemBaseTemplate = [this](std::filesystem::directory_entry de, std::string s, int po, bool iad) { addTemplate<ItemBase>(de, s, po, iad); };
        templateMap["itemfactionwarrant.tpl"] = itemBaseTemplate;
        templateMap["jewelry_amulet.tpl"] = itemBaseTemplate;
        templateMap["itemrandomsetformula.tpl"] = itemBaseTemplate;
        templateMap["itemartifactformula.tpl"] = itemBaseTemplate;
        templateMap["itemattributereset.tpl"] = itemBaseTemplate;
        templateMap["itemsetformula.tpl"] = itemBaseTemplate;
        templateMap["itemrelic.tpl"] = itemBaseTemplate;
        templateMap["itemdifficultyunlock.tpl"] = itemBaseTemplate;
        templateMap["itemdevotionreset.tpl"] = itemBaseTemplate;
        templateMap["itemusableskill.tpl"] = itemBaseTemplate;
        templateMap["itemnote.tpl"] = itemBaseTemplate;
        templateMap["itemenchantment.tpl"] = itemBaseTemplate;
        templateMap["jewelry_medal.tpl"] = itemBaseTemplate;
        templateMap["oneshot_potionhealth.tpl"] = itemBaseTemplate;
        templateMap["oneshot_potionmana.tpl"] = itemBaseTemplate;
        templateMap["oneshot_food.tpl"] = itemBaseTemplate;
        templateMap["oneshot_scroll.tpl"] = itemBaseTemplate;
        templateMap["oneshot_gold.tpl"] = itemBaseTemplate;
        templateMap["questitem.tpl"] = itemBaseTemplate;
        templateMap["itemartifact.tpl"] = itemBaseTemplate;
        templateMap["jewelry_ring.tpl"] = itemBaseTemplate;
        templateMap["weaponarmor_shield.tpl"] = itemBaseTemplate;
        templateMap["armor_waist.tpl"] = itemBaseTemplate;
        templateMap["armor_vestment.tpl"] = itemBaseTemplate;
        templateMap["armor_shoulders.tpl"] = itemBaseTemplate;
        templateMap["armor_legs.tpl"] = itemBaseTemplate;
        templateMap["armor_hands.tpl"] = itemBaseTemplate;
        templateMap["armor_feet.tpl"] = itemBaseTemplate;
        templateMap["armor_clothing.tpl"] = itemBaseTemplate;
        templateMap["armor_chest.tpl"] = itemBaseTemplate;
        templateMap["armor_head.tpl"] = itemBaseTemplate;
        templateMap["weapon_sword2h.tpl"] = itemBaseTemplate;
        templateMap["weapon_sword.tpl"] = itemBaseTemplate;
        templateMap["weapon_staff.tpl"] = itemBaseTemplate;
        templateMap["weapon_spear.tpl"] = itemBaseTemplate;
        templateMap["weapon_scepter.tpl"] = itemBaseTemplate;
        templateMap["weapon_ranged2h.tpl"] = itemBaseTemplate;
        templateMap["weapon_ranged1h.tpl"] = itemBaseTemplate;
        templateMap["weapon_offhand.tpl"] = itemBaseTemplate;
        templateMap["weapon_mace2h.tpl"] = itemBaseTemplate;
        templateMap["weapon_mace.tpl"] = itemBaseTemplate;
        templateMap["weapon_dagger.tpl"] = itemBaseTemplate;
        templateMap["weapon_axe2h.tpl"] = itemBaseTemplate;
        templateMap["weapon_axe.tpl"] = itemBaseTemplate;

        // Read directories of included mods first
        log_info << "Scanning: included mods\n";
        for (auto dir : _includedModDirs) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
                _threadProgress[0]++;
                std::string templateName = _parseTemplateName(entry);
                if (templateName != "") {
                    auto tempIt = templateMap.find(templateName);
                    if (tempIt != templateMap.end()) tempIt->second(entry, templateName, dir.length(), true);
                    else addTemplate<DBRBase>(entry, templateName, dir.length(), true);
                }
            }
        }

        // Read additional first to prevent creation of overriden files
        if (_addRecordsDirectory != "") {
            for (auto dir : _addSubDirectories) {
                log_info << "Scanning: " << _addRecordsDirectory + dir << "\n";
                for (const auto & entry : std::filesystem::recursive_directory_iterator(_addRecordsDirectory + dir)) {
                    _threadProgress[0]++;
                    std::string templateName = _parseTemplateName(entry);
                    if (templateName != "") {
                        auto tempIt = templateMap.find(templateName);
                        if (tempIt != templateMap.end()) tempIt->second(entry, templateName, _addRecordsDirectory.length(), false);
                        else addTemplate<DBRBase>(entry, templateName, _addRecordsDirectory.length());
                    }
                }
            }
        }

        for (auto dir : _subDirectories) {
            log_info << "Scanning: " << _recordsDirectory + dir << "\n";
            for (const auto & entry : std::filesystem::recursive_directory_iterator(_recordsDirectory + dir)) {
                _threadProgress[0]++;
                std::string templateName = _parseTemplateName(entry);
                if (templateName != "") {
                    auto tempIt = templateMap.find(templateName);
                    if (tempIt != templateMap.end()) tempIt->second(entry, templateName, _recordsDirectory.length(), false);
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
void FileManager::addTemplate(std::filesystem::directory_entry directoryEntry, std::string templateName, int pathOffset, bool isAlwaysDirty) {
    if (!std::is_base_of<DBRBase, T>::value) {
        throw std::runtime_error("Unknow type T in File Manager");
    }

    std::string pathAsValue = "records\\" + directoryEntry.path().string().substr(pathOffset);
    std::replace(pathAsValue.begin(), pathAsValue.end(), '\\', '/');
    if (_fileMap.find(pathAsValue) != _fileMap.end()) {
        log_info << "File overriden: " << pathAsValue << "\n";
        return;
    }

    DBRBase* temp = (DBRBase*)(new T(this, directoryEntry, templateName, isAlwaysDirty));
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
                dbrFile.open(filePath, std::ios::binary);
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
    dbrFile.open(entry.path(), std::ios::binary);
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