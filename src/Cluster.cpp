#include "Cluster.h"

#include <sys/stat.h>

void GetNodeLogFilenames(std::string const& SearchFilePath, std::vector<std::string>& OutFilePathes)
{
    OutFilePathes.clear();

    int NodeNameIndex = 0;

    char NodeFilePath[1024] = { '\0' };

    while (true)
    {
        std::snprintf(NodeFilePath, sizeof(NodeFilePath), "%s\\Node_%d.log", SearchFilePath.c_str(), NodeNameIndex);

        struct stat Buffer;
        if (stat(NodeFilePath, &Buffer) != 0)
        {
            break;
        }

        OutFilePathes.push_back(NodeFilePath);

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

void AdaptFrameCounter(int& FrameNumber, int& NodeFrameCounter, int FrameCounter)
{
    static const int FrameClipRange = 1000;

    FrameNumber += NodeFrameCounter;

    if (FrameNumber < FrameCounter)
    {
        NodeFrameCounter += FrameClipRange;
        FrameNumber += FrameClipRange;
    }
}

void Cluster::Compare(std::vector<FrameData> const& FrameData, ComparisonResult& Result)
{
    if (FrameData.empty())
    {
        return;
    }

    // TODO: Finish support for multiple nodes
    int Frame = FrameData[0].FrameNumber;

    std::stringstream OutputMsgs;

    for (auto const& [EntryHash, LineDataArray] : FrameData[0].Data)
    {
        if (FrameData[1].Data.find(EntryHash) == FrameData[1].Data.end())
        {
            OutputMsgs << RedColor << "[Desync]" << std::endl;
            OutputMsgs << RedColor << " -> " << WhiteColor << "Entries present only on Node[0]:" << std::endl;

            for (auto const& [EntryHash, LineDataArray] : FrameData[0].Data)
            {
                Result.TotalEntriesCount += LineDataArray.size();
                Result.AbsentEntriesCount += LineDataArray.size();

                for (auto const& EntryData : LineDataArray)
                {
                    std::string const& EntryName = EntryData.GetName();
                    std::string const& EntryInfo = EntryData.GetInfo();
                    std::string const& LogCategory = EntryData.GetCategory();

                    int LineNumber = EntryData.GetLineNumber();

                    OutputMsgs << RedColor << " -> "
                        << YellowColor << "Frame[" << Frame << "]"
                        << CyanColor << "[" << LogCategory << "]"
                        << WhiteColor << ", Ln[" << LineNumber << "]"
                        << ", Entry: " << EntryName
                        << ", Info: " << EntryInfo << std::endl;

                    Result.AddEntry(Frame, MsgType::Desync, EntryData);

                    SyncFramesState[Frame] = 0.5f;
                }
            }

            continue;
        }

        auto const& FoundIterator = FrameData[1].Data.find(EntryHash);
        auto const& LineDataArrayNode1 = FoundIterator->second;

        if (LineDataArray.size() != LineDataArrayNode1.size())
        {
            std::string const& EntryName = LineDataArray[0].GetName();
            std::string const& EntryInfo = LineDataArray[0].GetInfo();

            int LineNumber = LineDataArray[0].GetLineNumber();

            OutputMsgs << RedColor << "[Desync]" << std::endl;
            OutputMsgs << RedColor << "  -> " << WhiteColor << "Entries number are not the same for Entry: " << EntryName << std::endl;
            OutputMsgs << RedColor << "  -> " << WhiteColor << "Entries for Node[0]:" << std::endl;

            for (size_t i = 0; i < LineDataArray.size(); i++)
            {
                std::string const& EntryName = LineDataArray[i].GetName();
                std::string const& EntryInfo = LineDataArray[i].GetInfo();

                int LineNumber = LineDataArray[i].GetLineNumber();

                std::stringstream EntryStream;

                EntryStream << RedColor << "  -> "
                    << YellowColor << "Frame[" << Frame << "]"
                    << CyanColor << "[" << LineDataArray[i].GetCategory() << "]"
                    << WhiteColor << "Entry: " << EntryName
                    << ", Info:" << EntryInfo
                    << std::endl;

                Result.AddEntry(Frame, MsgType::Desync, LineDataArray[i]);
                
                SyncFramesState[Frame] = 0.5f;

                OutputMsgs << EntryStream.rdbuf();
            }

            OutputMsgs << RedColor << "  -> " << WhiteColor << "Entries for Node[1]:" << std::endl;

            for (size_t i = 0; i < LineDataArrayNode1.size(); i++)
            {
                std::string const& EntryName = LineDataArrayNode1[i].GetName();
                std::string const& EntryInfo = LineDataArrayNode1[i].GetInfo();

                size_t LineIdx = LineDataArrayNode1[i].GetLineNumber();

                std::stringstream EntryStream;

                EntryStream << RedColor << "  -> "
                    << YellowColor << "Frame[" << Frame << "]"
                    << CyanColor << "[" << LineDataArrayNode1[i].GetCategory() << "]"
                    << WhiteColor << "Entry: " << EntryName
                    << ", Info:" << EntryInfo
                    << std::endl;

                Result.AddEntry(Frame, MsgType::Desync, LineDataArrayNode1[i]);

                SyncFramesState[Frame] = 0.5f;

                OutputMsgs << EntryStream.rdbuf();
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

                std::stringstream EntryStream;

                EntryStream << RedColor << "[Desync]" << std::endl;
                EntryStream << YellowColor << "Frame[" << Frame << "]"
                    << WhiteColor << " Entries have different info field for Entry: " << LineDataArray[i].GetName() << std::endl;

                EntryStream << WhiteColor << "Ln[" << LineDataArray[i].GetLineNumber() 
                    << "] Info on Node[0]" << LineDataArray[i].GetInfo() << std::endl
                    << "Ln[" << LineDataArrayNode1[i].GetLineNumber() 
                    << "] Info on Node[1]" << LineDataArrayNode1[i].GetInfo() << std::endl;
                
                Result.AddEntry(Frame, MsgType::Desync, LineDataArray[i]);
                Result.AddEntry(Frame, MsgType::Desync, LineDataArrayNode1[i]);

                SyncFramesState[Frame] = 0.5f;

                OutputMsgs << EntryStream.rdbuf();
            }
            else
            {
                Result.IdenticalEntriesCount++;
                Result.TotalEntriesCount++;

                std::stringstream EntryStream;

                EntryStream << GreenColor << "[Sync]"
                    << YellowColor << "Frame[" << Frame << "]"
                    << CyanColor << "[" << LineDataArray[i].GetCategory() << "]"
                    << WhiteColor << "Entry: " << LineDataArray[i].GetName() << std::endl;

                Result.AddEntry(Frame, MsgType::Sync, LineDataArray[i]);

                SyncFramesState[Frame] = 1.0f;

                OutputMsgs << EntryStream.rdbuf();
            }
        }
    }

    std::cout << OutputMsgs.str();
}

void Cluster::ProcessDesyncFrameData(std::vector<FrameData> const& InFrameData,
    std::vector<size_t> const& NodeIndices, ComparisonResult& Result)
{
    for (size_t i = 0; i < InFrameData.size(); i++)
    {
        int FrameNumber = InFrameData[i].FrameNumber;
        auto const& FrameData = InFrameData[i].Data;

        size_t NodeNumber = NodeIndices[i];

        std::cout << RedColor << "[Desync]" << std::endl;
        std::cout << RedColor << " -> " << WhiteColor << "Entries present only on Node[" << NodeNumber << "]:" << std::endl;

        for (auto const& [ActorHash, EntryData] : FrameData)
        {
            for (auto const& Entry : EntryData)
            {
                std::stringstream EntryStream;

                EntryStream << RedColor << " -> " << YellowColor << "Frame[" << FrameNumber << "]"
                    << Entry << std::endl;

                Result.AddEntry(FrameNumber, MsgType::Desync, Entry);
                
                SyncFramesState[FrameNumber] = 0.5f;

                std::cout << EntryStream.rdbuf();
            }

            Result.AbsentEntriesCount += EntryData.size();
        }
    }
}

void Cluster::InsertFrameDataIntoCluster(NodeData& InNodeData, FrameData const& InFrameData)
{
    InNodeData.Data.insert({ InFrameData.FrameNumber, InFrameData });

    MaxFrameNumber = std::max(InFrameData.FrameNumber, MaxFrameNumber);

    if (SyncFramesState.size() < MaxFrameNumber)
    {
        SyncFramesState.resize(MaxFrameNumber);
        std::fill(SyncFramesState.begin(), SyncFramesState.end(), 0.0f);
    }

    for (auto const& [EntryHash, LineDataArray] : InFrameData.Data)
    {
        for (auto const& Entry : LineDataArray)
        {
            AllEntryNames.insert(Entry.GetName());
        }
    }
}

bool Cluster::LoadNodeData(std::string const& SearchFilePath)
{
    NodeFileNames.clear();
    AllEntryNames.clear();
    ClusterData.clear();

    SyncFramesState.reserve(8096);

    MaxFrameNumber = 0;

    GetNodeLogFilenames(SearchFilePath, NodeFileNames);

    if (NodeFileNames.empty())
    {
        std::cout << RedColor << "[Error] Can't find any log files" << WhiteColor << std::endl;
        return false;
    }

    size_t NumNodes = NodeFileNames.size();

    std::vector<LogReader> LogReaders(NumNodes);

    for (size_t i = 0; i < NumNodes; i++)
    {
        if (!LogReaders[i].Open(NodeFileNames[i]))
        {
            std::cout << RedColor << "[Error] Can't open log file: " << NodeFileNames[i] << WhiteColor << std::endl;
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

    std::vector<FrameData> FrameData(NumNodes);
    ClusterData.resize(NumNodes);

    for (size_t i = 0; i < ClusterData.size(); i++)
    {
        ClusterData[i].NodeNumber = static_cast<int>(i);
    }

    std::vector<int> NodeFrameCounter(NumNodes);

    for (size_t i = 0; i < NumNodes; i++)
    {
        NodeFrameCounter[i] = 0;
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
                    AdaptFrameCounter(FrameData[i].FrameNumber, NodeFrameCounter[i], FrameCounter);
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
                AdaptFrameCounter(FrameData[NodeNumber].FrameNumber, NodeFrameCounter[NodeNumber], FrameCounter);
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

        FrameCounter = std::min(FrameData[0].FrameNumber, FrameData[1].FrameNumber);

        if (bIncrementFrameCounter)
        {
            FrameCounter++;
        }
    }

    return true;
}

bool Cluster::CompareNodeData(ComparisonResult& Result)
{
    if (ClusterData.empty() || ClusterData[0].Data.empty())
    {
        return false;
    }

    size_t NumNodes = NodeFileNames.size();

    size_t MinFrameNumber = ClusterData[0].Data.begin()->first;
    size_t MaxFrameNumber = std::prev(ClusterData[0].Data.end())->first;

    std::vector<FrameData> FrameData(NumNodes);
    std::vector<size_t> NodeIndices(NumNodes);

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

        if (NumFramesFound != NumNodes)
        {
            ProcessDesyncFrameData(FrameData, NodeIndices, Result);
            continue;
        }

        Compare(FrameData, Result);
    }

    Result.SyncFramesState = SyncFramesState;

    return true;
}
