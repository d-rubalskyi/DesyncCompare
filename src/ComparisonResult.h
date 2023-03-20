#pragma once

#include <map>
#include <vector>
#include <sstream>

#include "EntryData.h"

enum class MsgType
{
    Desync,
    Sync
};

struct MsgEntry
{
    EntryData Entry;
    size_t FrameIdx;
    MsgType Type;
};

class ComparisonResult
{
    friend class Cluster;

public:
    void AddEntry(size_t FrameIdx, MsgType Type, EntryData Entry);

    void Print();

    void FilterByEntryName(std::string const& EntryName, std::vector<MsgEntry>& OutFilteredMsgs);
    void FilterByMsgType(MsgType Type, std::vector<MsgEntry>& OutFilteredMsgs);
    void FilterUniqueMsgs(std::vector<MsgEntry> const& InMsgs, std::vector<MsgEntry>& OutMsgs);

protected:
    size_t TotalEntriesCount = 0;
    size_t IdenticalEntriesCount = 0;
    size_t DifferentEntriesCount = 0;
    size_t AbsentEntriesCount = 0;

    std::map<size_t, std::vector<MsgEntry>> ComparisonMessages;
};
