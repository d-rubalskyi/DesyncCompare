#pragma once

#include <string>
#include <iostream>

#include "ConsoleColors.h"

class EntryData
{
public:
    EntryData() : LineNumber(-1) {};
    EntryData(int InLineNumber) : LineNumber(InLineNumber){ }

    bool ParseLine(std::string const& FileLine, int& OutFrame);
    
    std::string const& GetName() const { return EntryName; };
    std::string const& GetInfo() const { return EntryInfo; };
    std::string GetCategory() const { return EntryCategory; }

    int GetLineNumber() const { return LineNumber; }

    friend std::ostream& operator<<(std::ostream& os, EntryData const& InLineData)
    {
        std::string const& EntryName = InLineData.GetName();
        std::string const& EntryInfo = InLineData.GetInfo();
        std::string const& EntryCategory = InLineData.GetCategory();

        int LineNumber = InLineData.GetLineNumber();

        os << CyanColor << "[" << EntryCategory << "]"
            << WhiteColor << ", Ln[" << LineNumber << "]"
            << ", Actor:" << EntryName
            << ", Info: " << EntryInfo;

        return os;
    }
protected:
    std::string EntryName;
    std::string EntryInfo;
    std::string EntryCategory;

    int LineNumber;
};
