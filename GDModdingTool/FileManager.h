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

#include "Utils.h"

class DBRBase;

struct DBRData
{
    std::vector<DBRBase*> dbrs;
    std::unordered_map<std::string, size_t> fieldMap;
    std::mutex lock;
};

class FileManager {
    const int THREAD_COUNT = 16;
    std::vector<int> _threadProgress;
    std::vector<ThreadStatus> _threadStatus;
    int _progressTotal;

    std::string _recordsDirectory;
    std::string _addRecordsDirectory;
    std::string _modDirectory;
    std::vector<std::string> _includedModDirs;
    std::vector<std::string> _subDirectories;
    std::vector<std::string> _addSubDirectories;
    std::vector<std::string> _templateNames;
    std::unordered_map<std::string, DBRData*> _templateMap;
    std::unordered_map<std::type_index, std::vector<DBRBase*>> _typeMap;
    std::unordered_map<std::string, DBRBase*> _fileMap;
    std::unordered_map<std::string, int> _factionMap;
    void _scanFiles();
    void _save(int tnum, int size, std::vector<std::pair<DBRBase*, std::string>> temps);
    size_t _addField(DBRData* data, std::string field);
    std::string _parseTemplateName(std::filesystem::directory_entry entry);

public:
    FileManager(const Config& config);

    const int threadProgress() const {
        int total = 0;
        for (const auto tp : _threadProgress) total += tp;

        return total;
    }

    const ThreadStatus threadStatus() const {
        ThreadStatus status = ThreadStatus::NotRunning;
        for (const auto s : _threadStatus) {
            if (s == ThreadStatus::ThrownError) {
                return ThreadStatus::ThrownError;
            }
            else if (s == ThreadStatus::Running || (s == ThreadStatus::Completed && status == ThreadStatus::NotRunning)) {
                status = s;
            }
        }

        return status;
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

    const size_t getFieldIndex(std::string templateName, std::string fieldName) {
        auto it = _templateMap.find(templateName);
        if (it == _templateMap.end()) {
            throw std::runtime_error("Template does not exist");
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
    void addTemplate(std::filesystem::directory_entry directoryEntry, std::string templateName, int pathOffset, bool isAlwaysDirty = false);
    template <typename T>
    std::vector<DBRBase*> getFiles() {
        auto it = _typeMap.find(std::type_index(typeid(T)));
        if (it != _typeMap.end()) {
            return it->second;
        }

        return std::vector<DBRBase*>();
    }
};