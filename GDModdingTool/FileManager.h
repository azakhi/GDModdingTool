#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <mutex>

class DBRBase;

struct DBRData
{
    std::vector<DBRBase*> dbrs;
    std::unordered_map<std::string, int> fieldMap;
    std::mutex lock;
};

class FileManager {
    const int THREAD_COUNT = 16;
    std::vector<int> _threadProgress;
    std::vector<bool> _isThreadDone;
    int _progressTotal;

    std::string _recordsDirectory;
    std::string _modDirectory;
    std::vector<std::string> _subDirectories;
    std::vector<std::string> _templateNames;
    std::unordered_map<std::string, DBRData*> _templateMap;
    std::unordered_map<std::type_index, std::vector<DBRBase*>> _typeMap;
    std::unordered_map<std::string, DBRBase*> _fileMap;
    std::unordered_map<std::string, int> _factionMap;
    void _scanFiles();
    void _save(int tnum, int size, std::vector<DBRBase*> temps);
    int _addField(DBRData* data, std::string field);

public:
    FileManager(std::string recordsDirectory, std::string modDirectory, std::vector<std::string> subDirectories);

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

        int index = (int)_factionMap.size();
        _factionMap[path] = index;
        return index;
    }

    const int getFieldIndex(std::string templateName, std::string fieldName) {
        auto it = _templateMap.find(templateName);
        if (it == _templateMap.end()) {
            throw "Template does not exist";
        }

        // TODO: Check if locking frequently is a performance problem
        return _addField(_templateMap[templateName], fieldName);

        //auto fieldIt = _templateMap[templateName]->fieldMap.find(templateName);
        //if (fieldIt == _templateMap[templateName]->fieldMap.end()) {
        //    return _addField(_templateMap[templateName], fieldName);
        //}

        //return fieldIt->second;
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