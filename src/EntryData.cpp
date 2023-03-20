#include "EntryData.h"

bool EntryData::ParseLine(std::string const& FileLine, int& OutFrame)
{
    static std::string EntryNameMarker = "EntryName: ";
    static std::string TypeNameMarker = "EntryType: ";

    short Year = -1;
    short Month = -1;
    short Day = -1;

    short Hour = -1;
    short Minute = -1;
    short Second = -1;
    short NanoSecond = -1;

    sscanf_s(FileLine.c_str(), "[%hu.%hu.%hu-%hu.%hu.%hu:%hu][ %d]",
        &Year, &Month, &Day, &Hour, &Minute, &Second, &NanoSecond, &OutFrame);

    size_t StrEntryNameIndex = FileLine.find(EntryNameMarker);
    size_t StrStartEntryNameIndex = StrEntryNameIndex + EntryNameMarker.size();
    size_t StrEndEntryNameIndex = FileLine.find(",", StrStartEntryNameIndex);

    size_t StrTypeIndex = FileLine.find(TypeNameMarker);
    size_t StrStartTypeIndex = StrTypeIndex + TypeNameMarker.size();
    size_t StrEndTypeIndex = FileLine.find(",", StrStartTypeIndex);

    if ((StrStartEntryNameIndex == -1 || StrEndEntryNameIndex == -1) || 
        (StrStartTypeIndex == -1 || StrEndTypeIndex == -1))
    {
        return false;
    }

    EntryName = FileLine.substr(StrStartEntryNameIndex, StrEndEntryNameIndex - StrStartEntryNameIndex);
    EntryInfo = FileLine.substr(StrEndTypeIndex + 1, FileLine.size());
    EntryCategory = FileLine.substr(StrStartTypeIndex, StrEndTypeIndex - StrStartTypeIndex);

    return true;
}
