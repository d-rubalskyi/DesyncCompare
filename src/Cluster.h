#pragma once

#include <map>
#include <vector>
#include <set>

#include "LogReader.h"

struct NodeData
{
    int NodeNumber;

    // key - FrameNumber
    std::map<size_t, FrameData> Data;
};

class Cluster
{
public:
    bool ExtractNodeData();
    bool CompareNodeData(FrameComparisonData& TotalResult);

protected:
    void InsertFrameDataIntoCluster(NodeData& InNodeData, FrameData const& InFrameData);

protected:
    std::vector<NodeData> ClusterData;

    std::vector<std::string> NodeFileNames;
    std::set<std::string> AllEntryNames;
};
