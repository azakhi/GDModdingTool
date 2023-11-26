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

void parseConfigFile(Config& config);

int Logger::ErrorCount;

int main()
{
    Logger::ErrorCount = 0;
    try {
        Config config;
        parseConfigFile(config);
        if (config.isAddCaravanExtreme) {
            if (std::filesystem::exists("OptionalMods\\CaravanExtreme\\Parsed\\database\\records\\")) {
                config.includedModDirs.push_back("OptionalMods\\CaravanExtreme\\Parsed\\database\\records\\");
            }
            else {
                Print << "Error: Couldn't find CaravanExtreme\n";
                log_error << "CaravanExtreme directory doesn't exist\n";
            }
        }

        FileManager fileManager(config);

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        Print << "Scanning files\n";
        while (fileManager.threadStatus() == ThreadStatus::Running) {
            Sleep(100);
            TempPrint << fileManager.threadProgress() << " files scanned\n";
        }

        if (fileManager.threadStatus() == ThreadStatus::ThrownError) {
            throw std::runtime_error("Error occurred while scanning files");
        }

        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        Print << fileManager.threadProgress() << " files scanned ( " << std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count() << " sec )\n";

        auto templateNames = fileManager.getTemplateNames();
        Print << templateNames.size() << " template names found\n\n";

        Customizer customizer(&fileManager, config.commands);
        if (config.isAddCaravanExtreme) customizer.setupForCaravanExtreme();

        std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
        Print << "Pre-parsing required files\n";
        customizer.preParse();
        while (customizer.threadStatus() == ThreadStatus::Running) {
            Sleep(500);
            TempPrint << customizer.progress();
        }

        if (customizer.threadStatus() == ThreadStatus::ThrownError) {
            throw std::runtime_error("Error occurred while pre-parsing files");
        }

        std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
        Print << customizer.progressTotal() << " files parsed ( " << std::chrono::duration_cast<std::chrono::seconds>(t4 - t3).count() << " sec )\n\n";
        Print << "Running tasks\n";
        customizer.runTasks();

        std::chrono::high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();
        Print << "Saving files\n";
        fileManager.save();
        while (fileManager.threadStatus() == ThreadStatus::Running) {
            Sleep(500);
            TempPrint << fileManager.progress();
        }

        if (fileManager.threadStatus() == ThreadStatus::ThrownError) {
            throw std::runtime_error("Error occurred while saving files");
        }

        Print << fileManager.progressTotal() << " files saved\n";

        if (config.isAddCaravanExtreme) {
            Print << "Adding CaravanExtreme ..\n";
            if (std::filesystem::exists("OptionalMods\\CaravanExtreme\\CopyOnly\\")) {
                std::filesystem::copy("OptionalMods\\CaravanExtreme\\CopyOnly\\", config.modDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
            }
            else {
                Print << "Error: Couldn't find CaravanExtreme\n";
                log_error << "CaravanExtreme directory doesn't exist\n";
            }
        }

        if (config.isAddInventoryBagsAtStart) {
            Print << "Adding InventoryBagsAtStart ..\n";
            if (std::filesystem::exists("OptionalMods\\InventoryBagsAtStart\\CopyOnly\\")) {
                std::filesystem::copy("OptionalMods\\InventoryBagsAtStart\\CopyOnly\\", config.modDir, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
            }
            else {
                Print << "Error: Couldn't find InventoryBagsAtStart\n";
                log_error << "InventoryBagsAtStart directory doesn't exist\n";
            }
        }

        std::chrono::high_resolution_clock::time_point t6 = std::chrono::high_resolution_clock::now();
        Print << "Finished ( " << std::chrono::duration_cast<std::chrono::seconds>(t6 - t5).count() << " sec )\n";
        if (Logger::ErrorCount > 0) {
            Print << "\nThere were " << Logger::ErrorCount << " errors. Check logs in " << Logger::LogFileName() << "\n";
        }
    }
    catch (const std::exception& e) {
        log_error << e.what() << "\n";
        Print << "Error Occurred During Execution\n";
        Print << e.what() << "\n";
    }
    catch (const char* errorMessage) {
        log_error << errorMessage << "\n";
        Print << "Error Occurred During Execution\n";
        Print << errorMessage << "\n";
    }

    Print << "Press Enter to exit ..\n";
    getchar();
}

void parseConfigFile(Config& config) {
    if (!std::filesystem::exists("config.txt")) {
        throw std::runtime_error("Error: Couldn't find config.txt. You can create one quickly by copying one of examples and renaming it.");
    }

    std::ifstream configFile;
    configFile.open("config.txt");
    std::string line;
    while (std::getline(configFile, line)) {
        line = StringTrim(line);

        if (line == "$SubDirectories") {
            while (std::getline(configFile, line)) {
                line = StringTrim(line);
                if (line == "" || line[0] == '#') {
                    line = "";
                    continue;
                }
                else if (line[0] == '$') { // Let others catch
                    break;
                }
                else {
                    config.subDirs.push_back(line);
                    line = "";
                }
            }
        }

        if (line == "$AdditionalSubDirectories") {
            while (std::getline(configFile, line)) {
                line = StringTrim(line);
                if (line == "" || line[0] == '#') {
                    line = "";
                    continue;
                }
                else if (line[0] == '$') { // Let others catch
                    break;
                }
                else {
                    config.addSubDirs.push_back(line);
                    line = "";
                }
            }
        }

        if (line == "$Commands") {
            while (std::getline(configFile, line)) {
                line = StringTrim(line);
                if (line == "" || line[0] == '#') {
                    line = "";
                    continue;
                }
                else if (line[0] == '$') { // Let others catch
                    break;
                }
                else {
                    config.commands.push_back(line);
                    line = "";
                }
            }
        }

        if (line == "$AddInventoryBagsAtStart") {
            config.isAddInventoryBagsAtStart = true;
            line = "";
        }

        if (line == "$AddCaravanExtreme") {
            config.isAddCaravanExtreme = true;
            line = "";
        }

        if (line == "$RecordsDirectory") {
            line = "";
            while ((line == "" || line[0] == '#') && std::getline(configFile, line)) {
                line = StringTrim(line);
            }

            if (line != "" && line[0] != '#' && line[0] != '$') {
                if (line[line.size() - 1] != '\\') line.push_back('\\');
                config.recordsDir = line;
                line = "";
            }
        }

        if (line == "$AdditionalRecordsDirectory") {
            line = "";
            while ((line == "" || line[0] == '#') && std::getline(configFile, line)) {
                line = StringTrim(line);
            }

            if (line != "" && line[0] != '#' && line[0] != '$') {
                if (line[line.size() - 1] != '\\') line.push_back('\\');
                config.addRecordsDir = line;
                line = "";
            }
        }
        
        if (line == "$ModDirectory") {
            line = "";
            while ((line == "" || line[0] == '#') && std::getline(configFile, line)) {
                line = StringTrim(line);
            }

            if (line != "" && line[0] != '#' && line[0] != '$') {
                if (line[line.size() - 1] != '\\') line.push_back('\\');
                if (line.substr(line.size() - 8, 8) == "records\\") line = line.substr(0, line.size() - 8);
                if (line.substr(line.size() - 9, 9) == "database\\") line = line.substr(0, line.size() - 9);
                config.modDir = line;
                line = "";
            }
        }
        
        if (line != "" && line[0] != '#') {
            log_warning << "Found meaningless line in config file: " << line << "\n";
        }
    }
}