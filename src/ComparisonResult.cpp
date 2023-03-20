#include "ComparisonResult.h"

#include "ConsoleColors.h"

#include <iostream>
#include <set>

void ComparisonResult::AddEntry(size_t FrameIdx, MsgType Type, EntryData InEntry)
{
    MsgEntry Entry = { InEntry, FrameIdx, Type };
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
            if (Msg.Entry.GetName() == EntryName)
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

void ComparisonResult::FilterUniqueMsgs(std::vector<MsgEntry> const& InMsgs, std::vector<MsgEntry>& OutMsgs)
{
    // Key - Entry Name, Value - Unique messages for Entry
    std::map<std::string, std::set<std::string>> UniqueStrings;
    std::map<std::string, std::vector<MsgEntry>> UniqueMsgs;

    for (auto const& Msg : InMsgs)
    {
        auto const& EntryName = Msg.Entry.GetName();
        auto const& EntryInfo = Msg.Entry.GetInfo();

        if (UniqueStrings[EntryName].find(EntryInfo) == UniqueStrings[EntryName].end())
        {
            UniqueStrings[EntryName].insert(EntryInfo);
            UniqueMsgs[EntryName].push_back(Msg);
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
        std::cout << GreenColor << "OK" << WhiteColor << std::endl;
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
