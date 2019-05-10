#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>

class DBRBase;
struct Variable;

class FileManager {
    const int THREAD_COUNT = 16;
    std::vector<int> _threadProgress;
    std::vector<bool> _isThreadDone;
    int _progressTotal;

    std::string _gameDirectory;
    std::string _modDirectory;
    std::vector<std::string> _subDirectories;
    std::vector<std::string> _templateNames;
    std::unordered_map<std::string, std::vector<DBRBase*>> _templateMap;
    std::unordered_map<std::type_index, std::vector<DBRBase*>> _typeMap;
    std::unordered_map<std::string, DBRBase*> _fileMap;
    int _factionCount = 0;
    std::unordered_map<std::string, int> _factionMap;
    void _scanFiles();
    void _save(int tnum, int size, std::vector<DBRBase*> temps);

public:
    FileManager(std::string gameDirectory, std::string modDirectory, std::vector<std::string> subDirectories);

    const int threadProgress() const {
        int total = 0;
        for (const auto tp : _threadProgress) total += tp;

        return total;
    }

    const bool isWorkersDone() const {
        for (const auto itd : _isThreadDone) {
            if (!itd) return false;
        }

        return true;
    }

    const int progressTotal() const { return _progressTotal; }

    const std::string progress() const {
        int total = threadProgress();
        std::ostringstream os;
        os << std::fixed << std::setprecision(1) << "Progress: " << (100 * total / (float)(_progressTotal)) << "% (" << total << "/" << _progressTotal << ")\n";
        return os.str();
    }

    const int getFaction(std::string path) {
        auto it = _factionMap.find(path);
        if (it != _factionMap.end()) {
            return it->second;
        }

        _factionMap[path] = _factionCount;
        return _factionCount++;
    }

    const std::vector<std::string> getTemplateNames() const;
    DBRBase* getFile(std::string path) const;
    std::vector<DBRBase*> getFiles(std::string templateName) const;
    std::vector<DBRBase*> getFiles(std::vector<std::string> templateNames) const;
    std::vector<DBRBase*> getFiles(std::vector<std::type_index> typeIndexes) const;
    void modifyField(std::string templateName, std::string fieldName, std::function<std::string(std::string)> modifier);
    void modifyField(std::string templateName, std::vector<std::string> fieldNames, std::function<std::string(std::string)> modifier);
    void save();

    template <typename T>
    void addTemplate(std::filesystem::directory_entry directoryEntry, std::string templateName);
    template <typename T>
    std::vector<DBRBase*> getFiles() {
        auto it = _typeMap.find(std::type_index(typeid(T)));
        if (it != _typeMap.end()) {
            return it->second;
        }

        return std::vector<DBRBase*>();
    }
};