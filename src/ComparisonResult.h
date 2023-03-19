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

    std::string Msg;
    std::string EntryName;
};

class ComparisonResult
{
    friend class Cluster;

public:
    void AddMsgEntry(size_t FrameIdx, MsgType Type, std::string const& EntryName,
        std::string const& Msg);

    void Print();

    void FilterByEntryName(std::string const& EntryName, std::vector<MsgEntry>& OutFilteredMsgs);
    void FilterByMsgType(MsgType Type, std::vector<MsgEntry>& OutFilteredMsgs);

protected:
    size_t TotalEntriesCount = 0;
    size_t IdenticalEntriesCount = 0;
    size_t DifferentEntriesCount = 0;
    size_t AbsentEntriesCount = 0;

    std::map<size_t, std::vector<MsgEntry>> ComparisonMessages;
};
