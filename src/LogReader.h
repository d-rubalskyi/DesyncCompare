#pragma once

#include <string>
#include <sstream>

#include <fstream>

#include <unordered_map>
#include <map>

#include <vector>

#include "EntryData.h"

struct FrameData
{
    int FrameNumber = 0;

    // key - ActorHashName
    std::unordered_map<size_t, std::vector<EntryData>> Data;
};

class LogReader
{
protected:
    std::ifstream FileStream;
    std::string Line;

    EntryData PreviousEntryData;

    int LastFrameNumber = -1;
    int LineNumber = 1;

    bool bAddPreviousEntryData = false;

public:
    bool ReadNextFrame(FrameData& OutFrameData, int FrameCounter);

    bool Open(std::string const& FileName);
    void Close();  

    bool IsFileReadFinished() const { return !FileStream.is_open() || FileStream.eof(); }

protected:
    bool ReadNextLine(EntryData& Data, int& OutFrame);
};
