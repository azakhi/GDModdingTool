// GDModdingTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>

#include "ConsolePrinter.h"
#include "Utils.h"
#include "Customizer.h"
#include "FileManager.h"
#include "DBRFiles/DBRBase.h"

int main()
{
    std::ifstream configFile;
    configFile.open("config.txt");

    std::string gameDirectory = "";
    std::string modDirectory = "";
    std::vector<std::string> subDirectories;
    std::string line;
    while (std::getline(configFile, line)) {
        line = StringTrim(line);
        if (line == "" || line[0] == '#') {
            continue;
        }

        if (gameDirectory == "") {
            gameDirectory = line;
        }
        else if (modDirectory == "") {
            modDirectory = line;
        }
        else {
            subDirectories.push_back(line);
        }
    }

    FileManager fileManager(gameDirectory, modDirectory, subDirectories);

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    Print << "Scanning files\n";
    while (!fileManager.isWorkersDone()) {
        Sleep(100);
        TempPrint << fileManager.threadProgress() << " files scanned\n";
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    Print << fileManager.threadProgress() << " files scanned ( " << std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count() << " sec )\n";

    auto templateNames = fileManager.getTemplateNames();
    Print << templateNames.size() << " template names found\n\n";

    Customizer customizer(&fileManager);
    ////customizer.adjustExpRequirement(0.5f);
    //customizer.setDevotionPointsPerShrine(2);
    //customizer.setMaxDevotionPoints(75);
    //customizer.adjustGoldDrop(3.0f);
    //customizer.adjustLootAmount(2.0f);
    //customizer.adjustChampionChance(5.0f);
    //customizer.adjustSpawnAmount(2.0f);
    //customizer.adjustMonsterClassWeight(MonsterClass::Common, 0.33f);
    //customizer.adjustMonsterClassWeight(MonsterClass::Champion, 0.33f);
    //customizer.adjustMonsterClassWeight(MonsterClass::Hero, 5.0f);
    //customizer.increaseMonsterClassLimit(MonsterClass::Hero, 1);
    //customizer.removeDifficultyLimits();
    //customizer.adjustFactionRepRequirements(0.8f);
    //customizer.setItemStackLimit(TypePotion, 1000);

    customizer.adjustSpecificLootAmount(10.0f, std::vector<ItemType>({TypeQuest}), std::vector<ItemClass>({ ClassQuest }), true);

    std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
    Print << "Pre-parsing required files\n";
    customizer.preParse();
    while (!customizer.isWorkersDone()) {
        Sleep(500);
        TempPrint << customizer.progress();
    }

    std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    Print << customizer.progressTotal() << " files parsed ( " << std::chrono::duration_cast<std::chrono::seconds>(t4 - t3).count() << " sec )\n\n";
    Print << "Running tasks\n";
    customizer.runTasks();

    std::chrono::high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();
    Print << "Saving files\n";
    fileManager.save();
    while (!fileManager.isWorkersDone()) {
        Sleep(500);
        TempPrint << fileManager.progress();
    }

    Print << fileManager.progressTotal() << " files saved\n";
    std::chrono::high_resolution_clock::time_point t6 = std::chrono::high_resolution_clock::now();
    Print << "Finished ( " << std::chrono::duration_cast<std::chrono::seconds>(t6 - t5).count() << " sec )\n";
}