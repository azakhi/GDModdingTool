#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <functional>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include "../FileManager.h"
#include "Field.h"

enum LootSource { Chest, Enemy, All };
enum MonsterClass { NoClass = -1, Common, Champion, Hero, Quest, Boss };
enum ItemClass { ClassNone = -1, ClassCommon, ClassMagical, ClassRare, ClassEpic, ClassLegendary, ClassQuest };
enum ItemType { TypeNone = -1, TypeAmulet, TypeArmor, TypeBelt, TypeBlueprint, TypeComponent, TypeMedal, TypePotion, TypeQuest, TypeRelic, TypeRing, TypeShield, TypeWeapon };
enum AffixType { NoAffix, NormalAffix, RareAffix };

struct Loot
{
    DBRBase* file = nullptr;
    NumericField<int>* weight = nullptr;
    Field* name = nullptr;
};

struct Spawn
{
    DBRBase* monster = nullptr;
    NumericField<int>* weight = nullptr;
    NumericField<int>* limit = nullptr;
    NumericField<int>* maxPlayerLevel = nullptr;
    NumericField<int>* minPlayerLevel = nullptr;
};

class OrderedFieldMap {
    FileManager* _fileManager;
    std::string _templateName;
    std::vector<int> _fields;
    std::vector<Field*> _fieldsOrdered;

public:
    OrderedFieldMap(FileManager* fileManager, std::string templateName) : _fileManager(fileManager), _templateName(templateName) {};

    const std::vector<Field*> fieldsOrdered() const {
        return _fieldsOrdered;
    }

    bool contains(std::string key) const {
        int index = _fileManager->getFieldIndex(_templateName, key);
        return (index < _fields.size() && _fields[index] >= 0);
    }

    const Field* operator [](std::string s) const {
        int index = _fileManager->getFieldIndex(_templateName, s);
        if (index >= _fields.size() || _fields[index] < 0) {
            return nullptr;
        }

        return _fieldsOrdered[_fields[index]];
    }

    Field*& operator [](std::string s) {
        int index = _fileManager->getFieldIndex(_templateName, s);
        if (index >= _fields.size()) {
            _fields.insert(_fields.end(), index - _fields.size() + 1, -1);
        }

        if (_fields[index] < 0) { // add new to ordered
            _fields[index] = (int)_fieldsOrdered.size();
            _fieldsOrdered.push_back(nullptr);
        }

        return _fieldsOrdered[_fields[index]];
    }
};

class DBRBase {
    bool _isParsed = false;

protected:
    FileManager* _fileManager;
    std::filesystem::directory_entry _directoryEntry;
    std::string _templateName;
    OrderedFieldMap _fieldMap;

public:
    DBRBase(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName)
        : _directoryEntry(directoryEntry), _fileManager(fileManager), _templateName(templateName), _fieldMap(fileManager, templateName) {};
    virtual void parse();
    virtual void applyChanges();
    void addFieldIfNotExists(std::string fieldName, std::string value);
    void modifyField(std::string fieldName, std::function<std::string(std::string)> modifier);
    void modifyField(std::vector<std::string> fieldNames, std::function<std::string(std::string)> modifier);
    std::string text();

    const std::filesystem::directory_entry directoryEntry() const {
        return _directoryEntry;
    }

    const std::string templateName() const {
        return _templateName;
    }

    virtual const bool isDirty() const {
        for (const auto& f : _fieldMap.fieldsOrdered()) {
            if (f != nullptr && f->isModified()) return true;
        }

        return false;
    }

    static MonsterClass MonsterClassOf(const std::string value) {
        if (value == "Common") { return MonsterClass::Common; }
        else if (value == "Champion") { return MonsterClass::Champion; }
        else if (value == "Hero") { return MonsterClass::Hero; }
        else if (value == "Quest") { return MonsterClass::Quest; }
        else if (value == "Boss") { return MonsterClass::Boss; }

        return MonsterClass::NoClass;
    }

    static ItemClass ItemClassOf(const std::string value) {
        if (value == "Common") { return ItemClass::ClassCommon; }
        else if (value == "Magical") { return ItemClass::ClassMagical; }
        else if (value == "Rare") { return ItemClass::ClassRare; }
        else if (value == "Epic") { return ItemClass::ClassEpic; }
        else if (value == "Legendary") { return ItemClass::ClassLegendary; }
        else if (value == "Quest") { return ItemClass::ClassQuest; }

        return ItemClass::ClassNone;
    }

    static ItemType ItemTypeOf(const std::string value) {
        if (value == "jewelry_amulet.tpl" || value == "Amulet") { return ItemType::TypeAmulet; }
        else if (value == "armor_waist.tpl" || value == "Belt") { return ItemType::TypeBelt; }
        else if (value.substr(0,6) == "armor_" || value == "Armor") { return ItemType::TypeArmor; }
        else if (value == "itemrandomsetformula.tpl" || value == "itemartifactformula.tpl" || value == "itemsetformula.tpl" || value == "Blueprint") { return ItemType::TypeBlueprint; }
        else if (value == "itemrelic.tpl" || value == "Component") { return ItemType::TypeComponent; }
        else if (value == "jewelry_medal.tpl" || value == "Medal") { return ItemType::TypeMedal; }
        else if (value == "oneshot_potionhealth.tpl" || value == "oneshot_potionmana.tpl" || value == "oneshot_food.tpl" || value == "Potion") { return ItemType::TypePotion; }
        else if (value == "questitem.tpl" || value == "Quest") { return ItemType::TypeQuest; }
        else if (value == "itemartifact.tpl" || value == "Relic") { return ItemType::TypeRelic; }
        else if (value == "jewelry_ring.tpl" || value == "Ring") { return ItemType::TypeRing; }
        else if (value == "weaponarmor_shield.tpl" || value == "Shield") { return ItemType::TypeShield; }
        else if (value.substr(0, 7) == "weapon_" || value == "Weapon") { return ItemType::TypeWeapon; }

        return ItemType::TypeNone;
    }

    static bool IsRatioPreserved(const std::vector<Spawn> spawns) {
        if (spawns.size() < 2) {
            return true;
        }

        float ratio = -1;

        for (const auto& s : spawns) {
            int modifiedWeight = s.weight->numericModifiedValue();
            if (modifiedWeight == 0) {
                if (s.weight->numericValue() == 0) {
                    continue;
                }
                else {
                    return false;
                }
            }

            float r = s.weight->numericValue() / (float)modifiedWeight;
            if (ratio < 0) {
                ratio = r;
            }
            else if (ratio != r) {
                return false;
            }
        }

        return true;
    }
};