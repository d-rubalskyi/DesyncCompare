#include "EntryData.h"

bool EntryData::ParseLine(std::string const& FileLine, int& OutFrame)
{
    static std::string ActorNameMarker = "Actor: ";
  
    short Year = -1;
    short Month = -1;
    short Day = -1;

    short Hour = -1;
    short Minute = -1;
    short Second = -1;
    short NanoSecond = -1;

    sscanf_s(FileLine.c_str(), "[%hu.%hu.%hu-%hu.%hu.%hu:%hu][ %d]Log%s",
        &Year, &Month, &Day, &Hour, &Minute, &Second, &NanoSecond, &OutFrame, &LogCategory, (int)sizeof(LogCategory));

    // Kill ':' symbol
    size_t LogCategoryLen = strlen(LogCategory);

    if (LogCategoryLen > 0)
    {
        LogCategory[strlen(LogCategory) - 1] = '\0';
    }

    size_t StrActorNameIndex = FileLine.find(ActorNameMarker);
    size_t StrStartActorNameIndex = StrActorNameIndex + ActorNameMarker.size();
    size_t StrEndActorNameIndex = FileLine.find(",", StrStartActorNameIndex);

    if (StrStartActorNameIndex == -1 || StrEndActorNameIndex == -1)
    {
        return false;
    }

    ActorName = FileLine.substr(StrStartActorNameIndex, StrEndActorNameIndex - StrStartActorNameIndex);
    Info = FileLine.substr(StrEndActorNameIndex + 1, FileLine.size());

    return true;
}
