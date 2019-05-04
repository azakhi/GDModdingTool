#pragma once

#define NOMINMAX

#define Print ConsolePrinter::Printer(true)
#define TempPrint ConsolePrinter::Printer(false)

#include <iostream>
#include <sstream>
#include <Windows.h>

typedef std::ostream& (*manip) (std::ostream&);

class ConsolePrinter {
    std::string _perma = "";
    std::string _temp = "";
    bool _isPerma = true;

    ConsolePrinter() {}

public:
    ConsolePrinter(const ConsolePrinter&) = delete;
    void operator=(const ConsolePrinter&) = delete;

    void setPerma(bool val) {
        _isPerma = val;
        _temp = "";
    }

    void add(std::string s) {
        if (_isPerma) {
            _perma += s;
        }
        else {
            _temp += s;
        }
    }

    std::string text() { return _perma + _temp; }

    static ConsolePrinter& getInstance()
    {
        static ConsolePrinter instance;
        return instance;
    }

    static ConsolePrinter& Printer(bool isPerma) {
        auto& instance = ConsolePrinter::getInstance();
        instance.setPerma(isPerma);
        return instance;
    }
};

template <class T> ConsolePrinter& operator<< (ConsolePrinter& con, const T& x) {
    std::ostringstream os;
    os << x;
    con.add(os.str());

    system("cls");
    std::cout << con.text();
    return con;
}

ConsolePrinter& operator<< (ConsolePrinter& con, manip manipulator) {
    std::ostringstream os;
    os << manipulator;
    con.add(os.str());

    system("cls");
    std::cout << con.text();
    return con;
}