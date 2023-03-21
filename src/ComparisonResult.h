#pragma once

#include <map>
#include <vector>
#include <set>
#include <sstream>

#include "EntryData.h"

enum class MsgType
{
    All,
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
    void Clear();

    void FilterByEntryName(std::string const& EntryName, std::vector<MsgEntry>& OutFilteredMsgs) const;
    void FilterByMsgType(MsgType Type, std::vector<MsgEntry>& OutFilteredMsgs) const;
    void FilterUniqueMsgs(std::vector<MsgEntry> const& InMsgs, std::vector<MsgEntry>& OutMsgs) const;

    void GetEntryNames(std::vector<std::string>& OutEntryNames) const;
    void GetCategoryNames(std::vector<std::string>& OutCategoryNames) const;

protected:
    size_t TotalEntriesCount = 0;
    size_t IdenticalEntriesCount = 0;
    size_t DifferentEntriesCount = 0;
    size_t AbsentEntriesCount = 0;

    std::set<std::string> EntryNames;
    std::set<std::string> CategoryNames;
    std::map<size_t, std::vector<MsgEntry>> ComparisonMessages;
};
