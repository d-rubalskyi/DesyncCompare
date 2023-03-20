#pragma once

#include <map>
#include <vector>
#include <sstream>

enum class MsgType
{
    Desync,
    Sync
};

struct MsgEntry
{
    MsgType Type;
    
    size_t FrameIdx;
    size_t LineIdx;

    std::string EntryInfo;
    std::string EntryName;
};

class ComparisonResult
{
    friend class Cluster;

public:
    void AddMsgEntry(size_t FrameIdx, size_t LineIdx, MsgType Type,
            std::string const& EntryName, std::string const& EntryInfo);

    void Print();

    void FilterByEntryName(std::string const& EntryName, std::vector<MsgEntry>& OutFilteredMsgs);
    void FilterByMsgType(MsgType Type, std::vector<MsgEntry>& OutFilteredMsgs);
    void FilterUniqueMsgs(std::vector<MsgEntry> InMsgs, std::vector<MsgEntry>& OutMsgs);

protected:
    size_t TotalEntriesCount = 0;
    size_t IdenticalEntriesCount = 0;
    size_t DifferentEntriesCount = 0;
    size_t AbsentEntriesCount = 0;

    std::map<size_t, std::vector<MsgEntry>> ComparisonMessages;
};
