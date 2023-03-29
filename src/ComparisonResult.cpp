#include "ComparisonResult.h"

#include "ConsoleColors.h"

#include <iostream>
#include <set>

void ComparisonResult::AddEntry(size_t FrameIdx, std::vector<int> const& LineIdx, MsgType Type, EntryData const& InEntry)
{
    MsgEntry Entry = { LineIdx, InEntry, FrameIdx, Type };
    ComparisonMessages[FrameIdx].emplace_back(Entry);

    EntryNames.emplace(InEntry.GetName());
    CategoryNames.emplace(InEntry.GetCategory());

    std::size_t EntryHash = std::hash<std::string>{}(InEntry.GetName());

    if (SyncEntryState.find(EntryHash) == SyncEntryState.end())
    {
        SyncEntryState[EntryHash] = (Type == MsgType::Sync) ? true : false;
    }
    else
    {
        bool SyncState = SyncEntryState[EntryHash] && (Type == MsgType::Sync);
        SyncEntryState[EntryHash] = SyncState;
    }
}

void ComparisonResult::FilterByEntryName(std::string const& EntryName, 
    std::vector<MsgEntry>& OutFilteredMsgs) const
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

void ComparisonResult::FilterByMsgType(MsgType Type, std::vector<MsgEntry>& OutFilteredMsgs) const
{
    OutFilteredMsgs.clear();

    for (auto const& [FrameIdx, MsgArray] : ComparisonMessages)
    {
        for (auto const& Msg : MsgArray)
        {
            if ((Msg.Type == Type) || (Type == MsgType::All))
            {
                OutFilteredMsgs.push_back(Msg);
            }
        }
    }
}

void ComparisonResult::FilterUniqueMsgs(std::vector<MsgEntry> const& InMsgs, 
    std::vector<MsgEntry>& OutMsgs) const
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

void ComparisonResult::Clear()
{
    TotalEntriesCount = 0;
    IdenticalEntriesCount = 0;
    DifferentEntriesCount = 0;
    AbsentEntriesCount = 0;

    EntryNames.clear();
    CategoryNames.clear();
    SyncEntryState.clear();
    ComparisonMessages.clear();
}

void ComparisonResult::GetEntryData(std::map<std::string, bool>& OutEntryData) const
{
    OutEntryData.clear();

    for (std::string const& EntryName : EntryNames)
    {
        std::size_t EntryHash = std::hash<std::string>{}(EntryName);

        auto const& SyncIt = SyncEntryState.find(EntryHash);

        if (SyncIt != SyncEntryState.end())
        {
            OutEntryData[EntryName] = SyncEntryState.find(EntryHash)->second;
        }
    }
}

void ComparisonResult::GetCategoryNames(std::vector<std::string>& OutCategoryNames) const
{
    OutCategoryNames.clear();

    for (auto const& CategoryName : CategoryNames)
    {
        OutCategoryNames.push_back(CategoryName);
    }
}

void ComparisonResult::GetSyncFramesState(std::vector<float>& OutSyncFrames) const
{
    OutSyncFrames = SyncFramesState;
}
