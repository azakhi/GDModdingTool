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
enum ItemClass {ClassNone = -1, ClassCommon, ClassMagical, ClassRare, ClassEpic, ClassLegendary, ClassQuest };
enum ItemType {TypeNone = -1, TypeAmulet, TypeArmor, TypeBelt, TypeBlueprint, TypeComponent, TypeMedal, TypePotion, TypeQuest, TypeRelic, TypeRing, TypeShield, TypeWeapon };

struct Loot
{
    Template* file = nullptr;
    NumericField<int>* weight = nullptr;
    Field* name = nullptr;
};

struct Spawn
{
    Template* monster = nullptr;
    NumericField<int>* weight = nullptr;
    NumericField<int>* limit = nullptr;
    NumericField<int>* maxPlayerLevel = nullptr;
    NumericField<int>* minPlayerLevel = nullptr;
};

class Template {
    bool _isParsed;

protected:
    FileManager* _fileManager;
    std::filesystem::directory_entry _directoryEntry;
    std::string _templateName;
    std::unordered_map<std::string, std::pair<int, Field*>> _fields;
    std::vector<Field*> _fieldsOrdered;

public:
    Template(FileManager* fileManager, std::filesystem::directory_entry directoryEntry, std::string templateName);
    virtual void parse(bool isFullyParse = false, std::vector<std::string> fields = std::vector<std::string>());
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
        for (const auto& f : _fieldsOrdered) {
            if (f->isModified()) return true;
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
        if (value == "jewelry_amulet.tpl") { return ItemType::TypeAmulet; }
        else if (value == "armor_waist.tpl") { return ItemType::TypeBelt; }
        else if (value.substr(0,6) == "armor_") { return ItemType::TypeArmor; }
        else if (value == "itemrandomsetformula.tpl" || value == "itemartifactformula.tpl" || value == "itemsetformula.tpl") { return ItemType::TypeBlueprint; }
        else if (value == "itemrelic.tpl") { return ItemType::TypeComponent; }
        else if (value == "jewelry_medal.tpl") { return ItemType::TypeMedal; }
        else if (value == "oneshot_potionhealth.tpl" || value == "oneshot_potionmana.tpl" || value == "oneshot_food.tpl") { return ItemType::TypePotion; }
        else if (value == "questitem.tpl") { return ItemType::TypeQuest; }
        else if (value == "itemartifact.tpl") { return ItemType::TypeRelic; }
        else if (value == "jewelry_ring.tpl") { return ItemType::TypeRing; }
        else if (value == "weaponarmor_shield.tpl") { return ItemType::TypeShield; }
        else if (value.substr(0, 7) == "weapon_") { return ItemType::TypeWeapon; }

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