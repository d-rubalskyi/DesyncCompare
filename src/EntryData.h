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
    
    std::string const& GetActorName() const { return ActorName; };
    std::string const& GetInfo() const { return Info; };

    const char* GetLogCategory() const { return LogCategory; }
    int GetLineNumber() const { return LineNumber; }

    friend std::ostream& operator<<(std::ostream& os, EntryData const& InLineData)
    {
        std::string const& ActorName = InLineData.GetActorName();
        std::string const& ActorInfo = InLineData.GetInfo();
        const char* LogCategory = InLineData.GetLogCategory();

        int LineNumber = InLineData.GetLineNumber();

        os << CyanColor << "[" << LogCategory << "]"
            << WhiteColor << ", Ln[" << LineNumber << "]"
            << ", Actor:" << ActorName
            << ", Info: " << ActorInfo;

        return os;
    }
protected:
    std::string ActorName;
    std::string Info;

    // TODO: Optimize to Enum
    char LogCategory[32] = {'\0'};

    int LineNumber;
};
