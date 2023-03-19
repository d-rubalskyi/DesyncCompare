#include "Cluster.h"

#include <sys/stat.h>

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

// Find FrameData with minimal FrameCounter
int FindMinFrameCounterNodeIdx(std::vector<FrameData> const& FrameData)
{
    if (FrameData.empty())
    {
        return -1;
    }

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

    return NodeNumber;
}

void Compare(std::vector<FrameData> const& FrameData, FrameComparisonData& Result, std::vector<size_t> const& ActorFilter)
{
    if (FrameData.empty())
    {
        return;
    }

    // TODO: Finish support for multiple nodes
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

                    std::cout << RedColor << " -> "
                        << YellowColor << "Frame[" << Frame << "]"
                        << CyanColor << "[" << LogCategory << "]"
                        << WhiteColor << ", Ln[" << LineNumber << "]"
                        << ", Actor: " << ActorName
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
            std::cout << RedColor << "  -> " << WhiteColor << "Entries number are not the same for Actor: " << ActorName << std::endl;
            std::cout << RedColor << "  -> " << WhiteColor << "Entries for Node[0]:" << std::endl;

            for (size_t i = 0; i < LineDataArray.size(); i++)
            {
                std::string const& ActorName = LineDataArray[i].GetActorName();
                std::string const& ActorInfo = LineDataArray[i].GetInfo();

                int LineNumber = LineDataArray[i].GetLineNumber();

                std::cout << RedColor << "  -> "
                    << YellowColor << "Frame[" << Frame << "],"
                    << CyanColor << "[" << LineDataArray[i].GetLogCategory() << "]"
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
                    << CyanColor << "[" << LineDataArrayNode1[i].GetLogCategory() << "]"
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
                std::cout  << YellowColor << "Frame[" << Frame << "]" 
                    << WhiteColor << " Entries have different info field for Actor: " << LineDataArray[i].GetActorName() << std::endl;

                std::cout << WhiteColor << "Ln[" << LineDataArray[i].GetLineNumber() << "] Info on Node[0]" << LineDataArray[i].GetInfo() << std::endl
                    << "Ln[" << LineDataArrayNode1[i].GetLineNumber() << "] Info on Node[1]" << LineDataArrayNode1[i].GetInfo() << std::endl;
            }
            else
            {
                Result.IdenticalEntriesCount++;
                Result.TotalEntriesCount++;

                std::cout << GreenColor << "[Sync]"
                    << YellowColor << "Frame[" << Frame << "]"
                    << CyanColor << "[" << LineDataArray[i].GetLogCategory() << "]"
                    << WhiteColor << "Actor: " << LineDataArray[i].GetActorName() << std::endl;
            }
        }
    }
}

void ProcessDesyncFrameData(std::vector<FrameData> const& InFrameData, 
    std::vector<size_t> const& NodeIndices,
    std::vector<size_t> const& ActorFilter, FrameComparisonData& Result)
{
    for (size_t i = 0; i < InFrameData.size(); i++)
    {
        int FrameNumber = InFrameData[i].FrameNumber;
        auto const& Data = InFrameData[i].Data;

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

        size_t NodeNumber = NodeIndices[i];

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
    }
}

void Cluster::InsertFrameDataIntoCluster(NodeData& InNodeData, FrameData const& InFrameData)
{
    InNodeData.Data.insert({ InFrameData.FrameNumber, InFrameData });

    for (auto const& [ActorHash, LineDataArray] : InFrameData.Data)
    {
        for (auto const& Entry : LineDataArray)
        {
            AllEntryNames.insert(Entry.GetActorName());
        }
    }
}

bool Cluster::ExtractNodeData()
{
    NodeFileNames.clear();
    GetNodeLogFilenames(NodeFileNames);

    if (NodeFileNames.empty())
    {
        std::cout << RedColor << "[Error] Can't find any log files" << std::endl;
        return false;
    }

    size_t NumNodes = NodeFileNames.size();

    std::vector<LogReader> LogReaders(NumNodes);

    for (size_t i = 0; i < NumNodes; i++)
    {
        if (!LogReaders[i].Open(NodeFileNames[i]))
        {
            std::cout << RedColor << "[Error] Can't open log file: " << NodeFileNames[i] << std::endl;
            std::cin.ignore();

            return false;
        }
    }

    std::cout << WhiteColor << "\nStart parsing \n";

    for (size_t i = 0; i < NumNodes; i++)
    {
        std::cout << NodeFileNames[i] << std::endl;
    }

    std::cout << std::endl << std::endl;

    int FrameCounter = 0;

    FrameComparisonData TotalResult;

    std::vector<FrameData> FrameData(NumNodes);
    ClusterData.resize(NumNodes);

    for (size_t i = 0; i < ClusterData.size(); i++)
    {
        ClusterData[i].NodeNumber = static_cast<int>(i);
    }

    while (!IsAllLogsFinished(LogReaders))
    {
        size_t ReadFromLogCounter = 0;

        if (IsSameFrameCounter(FrameData))
        {
            for (int i = 0; i < NumNodes; i++)
            {
                if (LogReaders[i].ReadNextFrame(FrameData[i], FrameCounter))
                {
                    InsertFrameDataIntoCluster(ClusterData[i], FrameData[i]);

                    ReadFromLogCounter++;
                }
            }
        }
        else
        {
            int NodeNumber = FindMinFrameCounterNodeIdx(FrameData);

            if (LogReaders[NodeNumber].ReadNextFrame(FrameData[NodeNumber], FrameCounter))
            {
                InsertFrameDataIntoCluster(ClusterData[NodeNumber], FrameData[NodeNumber]);

                ReadFromLogCounter++;
            }
        }

        if (ReadFromLogCounter == 0)
        {
            break;
        }

        bool bIncrementFrameCounter = ReadFromLogCounter == NumNodes;
        bIncrementFrameCounter = bIncrementFrameCounter && IsSameFrameCounter(FrameData);

        FrameCounter = std::max(FrameData[0].FrameNumber, FrameData[1].FrameNumber);

        if (bIncrementFrameCounter)
        {
            FrameCounter++;
        }
    }

    return true;
}

bool Cluster::CompareNodeData(FrameComparisonData& TotalResult)
{
    if (ClusterData.empty())
    {
        return false;
    }

    size_t NumNodes = NodeFileNames.size();

    size_t MinFrameNumber = ClusterData[0].Data.begin()->first;
    size_t MaxFrameNumber = std::prev(ClusterData[0].Data.end())->first;

    std::vector<FrameData> FrameData(NumNodes);
    std::vector<size_t> NodeIndices(NumNodes);

    std::vector<size_t> ActorFilter;

    for (size_t FrameIdx = 0; FrameIdx < MaxFrameNumber; FrameIdx++)
    {
        FrameData.clear();
        NodeIndices.clear();

        size_t NumFramesFound = 0;

        for (size_t NodeIdx = 0; NodeIdx < NumNodes; NodeIdx++)
        {
            const auto& CurrentNodeData = ClusterData[NodeIdx].Data;
            const auto& CurrentFrameDataIt = CurrentNodeData.find(FrameIdx);

            if (CurrentFrameDataIt != CurrentNodeData.end())
            {
                FrameData.push_back(CurrentFrameDataIt->second);
                NodeIndices.push_back(NodeIdx);

                NumFramesFound++;
            }
        }

        if (NumFramesFound == 0)
        {
            continue;
        }

        FrameComparisonData Result;

        if (NumFramesFound != NumNodes)
        {
            ProcessDesyncFrameData(FrameData, NodeIndices, ActorFilter, Result);
            continue;
        }

        Compare(FrameData, Result, ActorFilter);

        TotalResult.Accumulate(Result);
    }

    return true;
}
