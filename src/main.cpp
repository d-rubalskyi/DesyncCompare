#include <fstream>
#include <string>

#include <sys/stat.h>
#include <cstdio>

#include "ConsoleColors.h"
#include "LogReader.h"
#include "EntryData.h"

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

void GetNodeLogFilenames(std::vector<std::string>& OutFilePathes)
{
    OutFilePathes.clear();

    int NodeNameIndex = 0;
    
    char NodeName[32] = { '\0' };

    while (true)
    {
        std::snprintf(NodeName, sizeof(NodeName), "Node_%d.log", NodeNameIndex);

        struct stat buffer;
        if (stat(NodeName, &buffer) != 0)
        {
            break;
        }

        OutFilePathes.push_back(NodeName);

        NodeNameIndex++;
    }
}

bool IsSameFrameCounter(std::vector<FrameData> const& FrameNodesData)
{
    if (FrameNodesData.size() == 1)
    {
        return true;
    }

    bool bSameFrameCounter = true;

    for (size_t i = 1; i < FrameNodesData.size(); i++)
    {
        bSameFrameCounter = bSameFrameCounter && (FrameNodesData[i].FrameNumber == FrameNodesData[i - 1].FrameNumber);
    }

    return bSameFrameCounter;
}

bool IsAllLogsFinished(std::vector<LogReader> const& Logs)
{
    bool bAllLogsFinished = true;

    for (auto const& Log : Logs)
    {
        bAllLogsFinished = bAllLogsFinished && Log.IsFinished();
    }

    return bAllLogsFinished;
}

bool IsAllowedToReadFromLog(FrameData const& CurrentFrameData, std::vector<FrameData> const& FrameNodesData)
{
    if (CurrentFrameData.FrameNumber == -1)
    {
        return false;
    }

    bool bAllowedToRead = true;

    for (auto const& FrameData : FrameNodesData)
    {
        bAllowedToRead = bAllowedToRead && CurrentFrameData.FrameNumber <= FrameData.FrameNumber;
    }

    return bAllowedToRead;
}

void Compare(std::vector<FrameData> FrameData, FrameComparisonData& Result, std::vector<size_t> const& ActorFilter)
{
    if (FrameData.empty())
    {
        return;
    }

    if (!IsSameFrameCounter(FrameData))
    {
        // Find FrameData with minimal FrameCounter
        int FrameNumber = FrameData[0].FrameNumber;
        int NodeNumber = 0;

        for (size_t i = 1; i < FrameData.size(); i++)
        {
            if (FrameData[i].FrameNumber < FrameData[i - 1].FrameNumber)
            {
                FrameNumber = FrameData[i].FrameNumber;
                NodeNumber = static_cast<int>(i);
            }
        }

        auto const& Data = FrameData[NodeNumber].Data;

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

    // TODO: Finish support for multiple nodes
    // TODO: Find out why records from Node[1] is absend when out of sync
    int Frame = FrameData[0].FrameNumber;

    for (auto const& [ActorHash, LineDataArray] : FrameData[0].Data)
    {
        if (!ActorHasPassedTheFilter(ActorHash, ActorFilter))
        {
            continue;
        }

        if (FrameData[1].Data.find(ActorHash) == FrameData[1].Data.end())
        {
            std::cout << RedColor << "[Desync]" << std::endl;
            std::cout << RedColor << " -> " << WhiteColor << "Entries present only on Node[0]:" << std::endl;

            for (auto const& [ActorHash, LineDataArray] : FrameData[0].Data)
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

        auto const& FoundIterator = FrameData[1].Data.find(ActorHash);
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
    std::vector<std::string> NodeFilenames;
    GetNodeLogFilenames(NodeFilenames);

    if (NodeFilenames.empty())
    {
        std::cout << RedColor << "[Error] Can't find any log files" << std::endl;
        std::cin.ignore();

        return -1;
    }

    size_t NumNodes = NodeFilenames.size();

    std::vector<LogReader> LogReaders(NumNodes);

    for (size_t i = 0; i < NumNodes; i++)
    {
        if (!LogReaders[i].Open(NodeFilenames[i]))
        {
            std::cout << RedColor << "[Error] Can't open log file: " << NodeFilenames[i] << std::endl;
            std::cin.ignore();

            return -1;
        }
    }

    std::cout << WhiteColor << "\nStart parsing \n";
    for (size_t i = 0; i < NumNodes; i++)
    {
        std::cout << NodeFilenames[i] << std::endl;
    }

    std::cout << std::endl << std::endl;

    int FrameCounter = 0;

    FrameComparisonData TotalResult;

    std::vector<FrameData> FrameData(NumNodes);

    std::vector<size_t> ActorFilter = 
    {
        std::hash<std::string>{}("BP_Mob_Minion_Stuart_Child_C_19")
    };

    while (!IsAllLogsFinished(LogReaders))
    {
        size_t ReadFromLogCounter = 0;

        for (size_t i = 0; i < NumNodes; i++)
        {
            // FIX: Increment only with minimal FrameCounter
            if (IsAllowedToReadFromLog(FrameData[i], FrameData))
            {
                if (LogReaders[i].ReadNextFrame(FrameData[i], FrameCounter))
                {
                    ReadFromLogCounter++;
                }
            }
        }

        if (ReadFromLogCounter == 0)
        {
            break;
        }

        bool bIncrementFrameCounter = ReadFromLogCounter == NumNodes;
        bIncrementFrameCounter = bIncrementFrameCounter && IsSameFrameCounter(FrameData);

        FrameComparisonData Result;
        Compare(FrameData, Result, ActorFilter);
        
        TotalResult.Accumulate(Result);

        FrameCounter = std::max(FrameData[0].FrameNumber, FrameData[1].FrameNumber);
        
        if (bIncrementFrameCounter)
        {
            FrameCounter++;
        }
    }

    for (size_t i = 0; i < NumNodes; i++)
    {
        LogReaders[i].Close();
    }

    TotalResult.Print();

    std::cout << endl << "Press Enter..." << endl;
    std::cin.ignore();

    return 0;
}
