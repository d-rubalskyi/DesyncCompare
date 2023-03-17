#pragma once

#include <string>
#include <fstream>

#include <unordered_map>
#include <vector>

#include "EntryData.h"

using namespace std;

struct FrameComparisonData
{
    size_t TotalEntriesCount = 0;
    size_t IdenticalEntriesCount = 0;
    size_t DifferentEntriesCount = 0;
    size_t AbsentEntriesCount = 0;

    void Print();
    void Accumulate(FrameComparisonData const& Data);
};

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
    
    EntryData PreviousEntryData;
    int LastFrameNumber = -1;

    std::string Line;
    int LineNumber = 0;

    bool bAddPreviousEntryData = false;
    bool bFrameOverflow = false;

public:
    bool ReadNextFrame(FrameData& OutFrameData, int FrameCounter);

    bool Open(std::string const& FileName);
    void Close();  

    bool inline IsEndOfFile() { return FileStream.eof(); }
    bool inline IsOpen() { return FileStream.is_open(); }
    bool inline IsFinished() { return bFrameOverflow || !FileStream.is_open() || FileStream.eof(); }

protected:
    bool ReadNextLine(EntryData& Data, int& OutFrame);
};
