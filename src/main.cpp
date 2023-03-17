#include <fstream>
#include <string>

#include "ConsoleColors.h"

#include "LogReader.h"
#include "EntryData.h"

struct EntryFilter
{
    std::vector<size_t> ActorFilter;
    std::vector<const char*> CategoryFilter;
};

bool ActorHasPassedTheFilter(size_t ActorHash, std::vector<size_t> const& ActorFilter)
{
    if (ActorFilter.empty())
    {
        return true;
    }

    if (std::find(ActorFilter.begin(), ActorFilter.end(), ActorHash) == ActorFilter.end())
    {
        return false;
    }

    return true;
}

void Compare(FrameData const& FrameData0, FrameData const& FrameData1, FrameComparisonData& Result, std::vector<size_t> const& ActorFilter)
{
    if (FrameData0.FrameNumber != FrameData1.FrameNumber)
    {
        int FrameNumber = std::min(FrameData0.FrameNumber, FrameData1.FrameNumber);
        int NodeNumber = (FrameNumber == FrameData0.FrameNumber) ? 0 : 1;

        auto const& Data = FrameData0.FrameNumber < FrameData1.FrameNumber ? FrameData0.Data : FrameData1.Data;

        bool bFilterHasPassed = ActorFilter.empty();

        for (auto const& [ActorHash, LineDataArray] : Data)
        {
            if (ActorHasPassedTheFilter(ActorHash, ActorFilter))
            {
                bFilterHasPassed = true;
                Result.TotalEntriesCount += LineDataArray.size();
            }
        }

        if (!bFilterHasPassed)
        {
            return;
        }

        std::cout << RedColor << "[Desync]" << std::endl;
        std::cout << RedColor << " -> " << WhiteColor << "Entries present only on Node[" << NodeNumber << "]:" << std::endl;

        for (auto const& [ActorHash, LineDataArray] : Data)
        {
            if (!ActorHasPassedTheFilter(ActorHash, ActorFilter))
            {
                continue;
            }

            for (auto const& LineData : LineDataArray)
            {
                std::cout << RedColor << " -> " << YellowColor << "Frame[" << FrameNumber << "]" 
                    << LineData << std::endl;
            }

            Result.AbsentEntriesCount += LineDataArray.size();
        }

        return;
    }

    int Frame = FrameData0.FrameNumber;

    for (auto const& [ActorHash, LineDataArray] : FrameData0.Data)
    {
        if (!ActorHasPassedTheFilter(ActorHash, ActorFilter))
        {
            continue;
        }

        if (FrameData1.Data.find(ActorHash) == FrameData1.Data.end())
        {
            std::cout << RedColor << "[Desync]" << std::endl;
            std::cout << RedColor << " -> " << WhiteColor << "Entries present only on Node[0]:" << std::endl;

            for (auto const& [ActorHash, LineDataArray] : FrameData0.Data)
            {
                if (!ActorHasPassedTheFilter(ActorHash, ActorFilter))
                {
                    continue;
                }

                Result.TotalEntriesCount += LineDataArray.size();
                Result.AbsentEntriesCount += LineDataArray.size();

                for (auto const& LineData : LineDataArray)
                {
                    std::string const& ActorName = LineData.GetActorName();
                    std::string const& ActorInfo = LineData.GetInfo();
                    const char* LogCategory = LineData.GetLogCategory();

                    int LineNumber = LineData.GetLineNumber();

                    cout << RedColor << " -> " 
                         << YellowColor << "Frame["<< Frame << "]"
                         << CyanColor << "[" << LogCategory << "]"
                         << WhiteColor << ", Ln[" << LineNumber << "]"
                         << ", Actor:" << ActorName 
                         << ", Info: " << ActorInfo << std::endl;
                }
            }

            continue;
        }

        auto const& FoundIterator = FrameData1.Data.find(ActorHash);
        auto const& LineDataArrayNode1 = FoundIterator->second;

        if (LineDataArray.size() != LineDataArrayNode1.size())
        {
            std::string const& ActorName = LineDataArray[0].GetActorName();
            std::string const& ActorInfo = LineDataArray[0].GetInfo();
            int LineNumber = LineDataArray[0].GetLineNumber();

            std::cout << RedColor << "[Desync]" << std::endl;
            std::cout << RedColor << "  -> " << WhiteColor << "Entries number are not the same for Actor:" << ActorName << std::endl;
            std::cout << RedColor << "  -> " << WhiteColor << "Entries for Node[0]:" << std::endl;

            for (size_t i = 0; i < LineDataArray.size(); i++)
            {
                std::string const& ActorName = LineDataArray[i].GetActorName();
                std::string const& ActorInfo = LineDataArray[i].GetInfo();

                int LineNumber = LineDataArray[i].GetLineNumber();

                std::cout << RedColor << "  -> "
                    << YellowColor << "Frame[" << Frame << "]," 
                    << WhiteColor << "Actor: " << ActorName
                    << ", Info:" << ActorInfo
                    << std::endl;
            }

            std::cout << RedColor << "  -> " << WhiteColor << "Entries for Node[1]:" << std::endl;
            for (size_t i = 0; i < LineDataArrayNode1.size(); i++)
            {
                std::string const& ActorName = LineDataArrayNode1[i].GetActorName();
                std::string const& ActorInfo = LineDataArrayNode1[i].GetInfo();

                int LineNumber = LineDataArrayNode1[i].GetLineNumber();

                std::cout << RedColor << "  -> "
                    << YellowColor << "Frame[" << Frame << "]"
                    << WhiteColor << "Actor: " << ActorName
                    << ", Info:" << ActorInfo
                    << std::endl;
            }

            Result.DifferentEntriesCount += LineDataArray.size();
            Result.DifferentEntriesCount += LineDataArrayNode1.size();

            Result.TotalEntriesCount += LineDataArray.size();
            Result.TotalEntriesCount += LineDataArrayNode1.size();

            continue;
        }

        for (size_t i = 0; i < LineDataArray.size(); i++)
        {
            if (LineDataArray[i].GetInfo() != LineDataArrayNode1[i].GetInfo())
            {
                Result.DifferentEntriesCount++;
                
                std::cout << RedColor << "[Desync]" << std::endl;
                std::cout << WhiteColor << "Entries have different Info field for Actor:" << LineDataArray[i].GetActorName() << std::endl;

                std::cout << "Ln[" << LineDataArray[i].GetLineNumber() << "], Info on Node[0]" << LineDataArray[i].GetInfo()
                   << "Info on Node[1]" << LineDataArrayNode1[i].GetInfo() << std::endl;
            }
            else
            {
                Result.IdenticalEntriesCount++;
                Result.TotalEntriesCount++;

                std::cout << GreenColor << "[Sync]" 
                    << YellowColor << "Frame[" << Frame << "]"
                    << CyanColor << "[" << LineDataArray[i].GetLogCategory() << "]"
                    << WhiteColor << "Actor:" << LineDataArray[i].GetActorName() << std::endl;
            }
        }
    }
}

int main()
{
    std::string Node0 = "Node_0.log";
    std::string Node1 = "Node_1.log";

    std::cout << WhiteColor << "\nStart parsing \n" << Node0 << std::endl << Node1 << "\n...\n";

    LogReader LogNode0;
    LogReader LogNode1;

    if (!LogNode0.Open(Node0) || !LogNode1.Open(Node1))
    {
        std::cout << "\nCan't open log files \n";
        std::cin.ignore();

        return -1;
    }

    int FrameCounter = 0;

    FrameComparisonData TotalResult;

    FrameData FrameDataNode0;
    FrameData FrameDataNode1;

    std::vector<size_t> ActorFilter = 
    {
        std::hash<std::string>{}("BP_Boss_Stronghold_C_1")
    };

    while (!LogNode0.IsFinished() && !LogNode1.IsFinished())
    {
        bool bReadFrameFromNode0 = FrameDataNode0.FrameNumber <= FrameDataNode1.FrameNumber;
        bool bReadFrameFromNode1 = FrameDataNode1.FrameNumber <= FrameDataNode0.FrameNumber;

        if (bReadFrameFromNode0)
        {
            LogNode0.ReadNextFrame(FrameDataNode0, FrameCounter);
        }

        if (bReadFrameFromNode1)
        {
            LogNode1.ReadNextFrame(FrameDataNode1, FrameCounter);
        }
        
        bool bIncrementFrameCounter = bReadFrameFromNode0 && bReadFrameFromNode1;
        bIncrementFrameCounter = bIncrementFrameCounter && (FrameDataNode1.FrameNumber == FrameDataNode0.FrameNumber);

        FrameComparisonData Result;
        Compare(FrameDataNode0, FrameDataNode1, Result, ActorFilter);
        
        TotalResult.Accumulate(Result, FrameCounter);

        FrameCounter = std::max(FrameDataNode0.FrameNumber, FrameDataNode1.FrameNumber);
        
        if (bIncrementFrameCounter)
        {
            FrameCounter++;
        }
    }

    LogNode0.Close();
    LogNode1.Close();

    TotalResult.Print();

    std::cout << endl << "Press Enter..." << endl;
    std::cin.ignore();

    return 0;
}
