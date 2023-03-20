#include "ComparisonResult.h"

#include "ConsoleColors.h"

#include <iostream>
#include <set>

void ComparisonResult::AddMsgEntry(size_t FrameIdx, size_t LineIdx, MsgType Type,
    std::string const& EntryName, std::string const& EntryInfo)
{
    MsgEntry Entry = { Type, FrameIdx, LineIdx, EntryInfo, EntryName };

    ComparisonMessages[FrameIdx].emplace_back(Entry);
}

void ComparisonResult::FilterByEntryName(std::string const& EntryName, 
    std::vector<MsgEntry>& OutFilteredMsgs)
{
    OutFilteredMsgs.clear();

    for (auto const& [FrameIdx, MsgArray] : ComparisonMessages)
    {
        for (auto const& Msg : MsgArray)
        {
            if (Msg.EntryName == EntryName)
            {
                OutFilteredMsgs.push_back(Msg);
            }
        }
    }
}

void ComparisonResult::FilterByMsgType(MsgType Type, std::vector<MsgEntry>& OutFilteredMsgs)
{
    OutFilteredMsgs.clear();

    for (auto const& [FrameIdx, MsgArray] : ComparisonMessages)
    {
        for (auto const& Msg : MsgArray)
        {
            if (Msg.Type == Type)
            {
                OutFilteredMsgs.push_back(Msg);
            }
        }
    }
}

void ComparisonResult::FilterUniqueMsgs(std::vector<MsgEntry> InMsgs, std::vector<MsgEntry>& OutMsgs)
{
    // Key - Entry Name, Value - Unique messages for Entry
    std::map<std::string, std::set<std::string>> UniqueStrings;
    std::map<std::string, std::vector<MsgEntry>> UniqueMsgs;

    for (auto const& Msg : InMsgs)
    {
       if (UniqueStrings[Msg.EntryName].find(Msg.EntryInfo) == UniqueStrings[Msg.EntryName].end())
       {
           UniqueStrings[Msg.EntryName].insert(Msg.EntryInfo);
           UniqueMsgs[Msg.EntryName].push_back(Msg);
       }
    }

    OutMsgs.clear();

    for (auto const& [EntryName, MsgArray] : UniqueMsgs)
    {
        for (auto const& Msg : MsgArray)
        {
            OutMsgs.push_back(Msg);
        }
    }
}

void ComparisonResult::Print()
{
    std::cout << std::endl << "Final Results: ";

    if (TotalEntriesCount == IdenticalEntriesCount)
    {
        std::cout << GreenColor << "OK" << WhiteColor;
    }
    else
    {
        std::cout << RedColor << "FAILED" << WhiteColor << std::endl;
    }

    std::cout << "  Total Entries: " << TotalEntriesCount << std::endl;
    std::cout << "  Identical Entries: " << IdenticalEntriesCount << std::endl;
    std::cout << "  Different Entries: " << DifferentEntriesCount << std::endl;
    std::cout << "  Absent Entries: " << AbsentEntriesCount << std::endl;
}
