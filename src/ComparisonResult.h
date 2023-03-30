#pragma once

#include <map>
#include <unordered_map>

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
    std::vector<int> LineIndices;
    EntryData Entry;
    size_t FrameIdx;
    MsgType Type;
};

class ComparisonResult
{
    friend class Cluster;

public:
    void AddEntry(size_t FrameIdx, std::vector<int> const& LineIdx, MsgType Type, EntryData const& Entry);

    void Print(std::ostream& Stream) const;
    void Clear();

    void FilterByEntryName(std::string const& EntryName, std::vector<MsgEntry>& OutFilteredMsgs) const;
    void FilterByMsgType(MsgType Type, std::vector<MsgEntry>& OutFilteredMsgs) const;
    void FilterUniqueMsgs(std::vector<MsgEntry> const& InMsgs, std::vector<MsgEntry>& OutMsgs) const;

    void GetEntryData(std::map<std::string, bool>& OutEntryData) const;
    void GetCategoryNames(std::vector<std::string>& OutCategoryNames) const;
    void GetSyncFramesState(std::vector<float>& OutSyncFrames) const;

protected:
    size_t TotalEntriesCount = 0;
    size_t IdenticalEntriesCount = 0;
    size_t DifferentEntriesCount = 0;
    size_t AbsentEntriesCount = 0;

    std::set<std::string> EntryNames;
    std::set<std::string> CategoryNames;

    std::vector<float> SyncFramesState;

    std::unordered_map<size_t, bool> SyncEntryState;
    std::map<size_t, std::vector<MsgEntry>> ComparisonMessages;
};
