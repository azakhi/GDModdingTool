#pragma once

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>
#include <algorithm>

#include "../Logger.h"

#ifndef log_info
#define log_info (*Logger::logger())
#define log_warning (*Logger::logger())<<"WARNING: "
#define log_debug (*Logger::logger())<<"DEBUG: "
#define log_error (*Logger::logger())<<"ERROR: "
#endif // !log_info


class Field {
    std::string _value;
    std::string* _modifiedValue = nullptr;
protected:
    std::string _name;
    bool _neverModified;
public:

    Field(std::string name, std::string value = "", bool neverModified = false) {
        _name = name;
        _value = value;
        _neverModified = neverModified;
    }
    
    const std::string name() const { return _name; }
    virtual const std::string value() const { return _value; }
    virtual const std::string modifiedValue() const { return _modifiedValue == nullptr ? "" : *_modifiedValue; }

    virtual void setModifiedValue(std::string value) {
        if (_modifiedValue == nullptr) {
            _modifiedValue = new std::string();
        }

        *_modifiedValue = value;
    }

    virtual bool isModified() const {
        return !_neverModified && modifiedValue() != "" && value() != modifiedValue();
    }

    virtual std::string toString() const {
        return modifiedValue() != "" ? modifiedValue() : value();
    }
};

class NumericFieldBase : public Field {
protected:
    float _modifiedValue;
    float _min;
    float _max;

public:
    NumericFieldBase(std::string name, std::string value, bool neverModified = false, float min = 0.0f, float max = FLT_MAX) : Field(name, value, neverModified) {
        _modifiedValue = std::stof(value);
        _min = min;
        _max = max;
    }

    void setModifiedValue(std::string value) {
        setModifiedValue(std::stof(value));
    }

    void setModifiedValue(float value) {
        _modifiedValue = std::min(_max, std::max(_min, value));
    }
};

template <typename T>
class NumericField : public NumericFieldBase {
    T _value;
public:
    NumericField(std::string name, std::string value, bool neverModified = false, float min = 0.0f, float max = FLT_MAX)
        : NumericFieldBase(name, value, neverModified, min, max) {
        if (!std::is_integral<T>::value && !std::is_floating_point<T>::value) { throw "Invalid type"; }
        _value = (T)_modifiedValue;
    }

    T numericValue() const { return _value; }
    
    bool isModified() const {
        return !_neverModified && numericValue() != numericModifiedValue();
    }

    std::string toString() const {
        return std::to_string(numericModifiedValue());
    }

    template <typename U = T>
    std::enable_if_t<!std::is_integral<U>::value, U> numericModifiedValue() const {
        return _modifiedValue;
    }

    template <typename U = T>
    std::enable_if_t<std::is_integral<U>::value, U> numericModifiedValue() const {
        return (U)(_modifiedValue + 0.5f);
    }
};

template <class F, class = std::enable_if_t<std::is_base_of<Field, F>::value, F>>
class ArrayField : public Field {
    std::vector<F> _values;
public:
    ArrayField(std::string name, std::string value, bool neverModified = false, float min = 0.0f, float max = FLT_MAX) : Field(name, value, neverModified) {
        std::stringstream ss(value);
        std::string item = "";
        while (std::getline(ss, item, ';')) {
            if (std::is_base_of<NumericFieldBase, F>::value) {
                _values.push_back(F(name, item, false, min, max));
            }
            else {
                _values.push_back(F(name, item));
            }
        }
    }

    const std::string modifiedValue() const {
        std::string s;
        for (const auto& v : _values) {
            s += ";" + v.modifiedValue();
        }

        return s.substr(1);
    }

    std::vector<F>& values() {
        return _values;
    }

    float totalValue() const {
        float total = 0.0f;
        for (const auto& v : _values) total += (float)(v.numericValue());
        return total;
    }

    float totalModifiedValue() const {
        float total = 0.0f;
        for (const auto& v : _values) total += (float)(v.numericModifiedValue());
        return total;
    }

    void setModifiedValue(std::string value) {
        _values.clear();
        std::stringstream ss(value);
        std::string item = "";
        while (std::getline(ss, item, ';')) {
            F v(name(), item);
            _values.push_back(v);
        }
    }

    bool isModified() const {
        if (_neverModified) return false;

        for (const auto& v : _values) {
            if (v.isModified()) {
                return true;
            }
        }

        return false;
    }

    std::string toString() const {
        std::string s;
        for (const auto& v : _values) {
            s += ";" + v.toString();
        }

        return s.substr(1);
    }
};