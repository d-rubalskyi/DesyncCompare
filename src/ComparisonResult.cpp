#include "ComparisonResult.h"

#include "ConsoleColors.h"

#include <iostream>

void ComparisonResult::AddMsgEntry(size_t FrameIdx, MsgType Type,
    std::string const& EntryName, std::string const& Msg)
{
    MsgEntry Entry = { Type, Msg, EntryName };

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

    std::cout << "   Total Entries: " << TotalEntriesCount << std::endl;
    std::cout << "   Identical Entries: " << IdenticalEntriesCount << std::endl;
    std::cout << "   Different Entries: " << DifferentEntriesCount << std::endl;
    std::cout << "   Absent Entries: " << AbsentEntriesCount << std::endl;
}
