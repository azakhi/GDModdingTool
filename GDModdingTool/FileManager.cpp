#include "FileManager.h"

#define NOMINMAX

#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <thread>
#include <Windows.h>

#include "Templates/Template.h"
#include "Templates/FixedItemLoot.h"
#include "Templates/Monster.h"
#include "Templates/LootMasterTable.h"
#include "Templates/ProxyPool.h"
#include "Templates/ItemBase.h"
#include "Templates/DynWeightAffixTable.h"
#include "Templates/LevelTable.h"

#include <iostream>

FileManager::FileManager(std::string gameDirectory, std::string modDirectory, std::vector<std::string> subDirectories) {
    _progressTotal = 0;
    _threadProgress.resize(THREAD_COUNT, 0);
    _isThreadDone.resize(THREAD_COUNT, false);
    _gameDirectory = gameDirectory;
    _modDirectory = modDirectory;
    _subDirectories = subDirectories;

    auto t = std::thread(&FileManager::_scanFiles, this);
    t.detach();
}

const std::vector<std::string> FileManager::getTemplateNames() const {
    return _templateNames;
}

Template* FileManager::getFile(std::string path) const {
    auto it = _fileMap.find(path);
    if (it != _fileMap.end()) {
        return it->second;
    }

    return nullptr;
}

std::vector<Template*> FileManager::getFiles(std::string templateName) const {
    auto it = _templateMap.find(templateName);
    if (it != _templateMap.end()) {
        return it->second;
    }

    return std::vector<Template*>();
}

std::vector<Template*> FileManager::getFiles(std::vector<std::string> templateNames) const {
    std::vector<Template*> files;

    for (const auto& tname : templateNames) {
        auto it = _templateMap.find(tname);
        if (it != _templateMap.end()) {
            files.insert(files.end(), it->second.begin(), it->second.end());
        }
    }


    return files;
}

std::vector<Template*> FileManager::getFiles(std::vector<std::type_index> typeIndexes) const {
    std::vector<Template*> files;

    for (const auto& tindex : typeIndexes) {
        auto it = _typeMap.find(tindex);
        if (it != _typeMap.end()) {
            files.insert(files.end(), it->second.begin(), it->second.end());
        }
    }


    return files;
}

void FileManager::_scanFiles() {
    _threadProgress[0] = 0;
    std::fill(_isThreadDone.begin()+1, _isThreadDone.end(), true);
    _isThreadDone[0] = false;

    std::unordered_map<std::string, std::function<void(std::filesystem::directory_entry, std::string)>> templateMap;
    templateMap["fixeditemloot.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<FixedItemLoot>(de, s); };
    templateMap["monster.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<Monster>(de, s); };
    templateMap["lootmastertable.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<LootMasterTable>(de, s); };
    templateMap["proxypool.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ProxyPool>(de, s); };
    templateMap["lootitemtable_dynweighted_dynaffix.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<DynWeightAffixTable>(de, s); };
    templateMap["leveltable.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<LevelTable>(de, s); };
    // Item Templates
    templateMap["itemfactionwarrant.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["jewelry_amulet.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["itemrandomsetformula.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["itemartifactformula.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["itemsetformula.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["itemrelic.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["jewelry_medal.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["oneshot_potionhealth.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["oneshot_potionmana.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["oneshot_food.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["questitem.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["itemartifact.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["jewelry_ring.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weaponarmor_shield.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_waist.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_vestment.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_shoulders.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_legs.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_hands.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_feet.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_clothing.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_chest.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["armor_head.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_sword2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_sword.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_staff.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_spear.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_scepter.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_ranged2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_ranged1h.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_offhand.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_mace2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_mace.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_dagger.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_axe2h.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };
    templateMap["weapon_axe.tpl"] = [this](std::filesystem::directory_entry de, std::string s) { addTemplate<ItemBase>(de, s); };

    for (auto dir : _subDirectories) {
        for (const auto & entry : std::filesystem::recursive_directory_iterator(_gameDirectory + "\\" + dir)) {
            _threadProgress[0]++;
            std::string extension = entry.path().extension().string();
            if (extension != ".dbr") {
                continue;
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

            if (templateName == "") {
                continue;
            }

            if (_templateMap.find(templateName) == _templateMap.end()) {
                _templateMap[templateName] = std::vector<Template*>();
                _templateNames.push_back(templateName);
            }
            
            auto tempIt = templateMap.find(templateName);
            if (tempIt != templateMap.end()) tempIt->second(entry, templateName);
            else addTemplate<Template>(entry, templateName);
        }
    }

    _isThreadDone[0] = true;
}

void FileManager::modifyField(std::string templateName, std::string fieldName, std::function<std::string(std::string)> modifier) {
    if (_templateMap.find(templateName) == _templateMap.end()) {
        return;
    }

    for (auto temp : _templateMap[templateName]) {
        temp->modifyField(fieldName, modifier);
    }
}

void FileManager::modifyField(std::string templateName, std::vector<std::string> fieldNames, std::function<std::string(std::string)> modifier) {
    if (_templateMap.find(templateName) == _templateMap.end()) {
        return;
    }

    for (auto temp : _templateMap[templateName]) {
        temp->modifyField(fieldNames, modifier);
    }
}

template <typename T>
void FileManager::addTemplate(std::filesystem::directory_entry directoryEntry, std::string templateName) {
    if (!std::is_base_of<Template, T>::value) {
        throw "Unknow type T in File Manager";
    }

    Template* temp = (Template*)(new T(this, directoryEntry, templateName));
    std::type_index typeIndex(typeid(T));
    if (_typeMap.find(typeIndex) == _typeMap.end()) _typeMap[typeIndex] = std::vector<Template*>();
    _typeMap[typeIndex].push_back(temp);
    _templateMap[templateName].push_back(temp);
    std::string pathAsValue = "records" + directoryEntry.path().string().substr(_gameDirectory.length());
    std::replace(pathAsValue.begin(), pathAsValue.end(), '\\', '/');
    _fileMap[pathAsValue] = temp;
}

void FileManager::save() {
    std::vector<Template*> dirtyFiles;
    for (auto&[key, val] : _templateMap) {
        for (auto& file : val) {
            if (file->isDirty()) {
                dirtyFiles.push_back(file);
            }
        }
    }

    _progressTotal = dirtyFiles.size();
    int size = dirtyFiles.size() / _threadProgress.size() + 1;
    for (size_t i = 0; i < _threadProgress.size(); i++) {
        auto t = std::thread(&FileManager::_save, this, i, size, dirtyFiles);
        t.detach();
    }
}

void FileManager::_save(int tnum, int size, std::vector<Template*> temps) {
    int end = (tnum + 1) * size;
    if ((int)(temps.size()) < end) {
        end = (int)(temps.size());
    }

    _isThreadDone[tnum] = false;
    _threadProgress[tnum] = 0;
    for (int i = (tnum * size); i < end; i++) {
        auto temp = temps[i];
        if (temp->isDirty()) {
            std::string filePath = temp->directoryEntry().path().string().substr(_gameDirectory.length());
            filePath = _modDirectory + filePath;
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
    _isThreadDone[tnum] = true;
}