#include <cstdio>

#include "Cluster.h"

int main()
{
    Cluster NodeCluster;
    ComparisonResult Results;

    NodeCluster.LoadNodeData();
    NodeCluster.CompareNodeData(Results);

    Results.Print();

    std::cin.ignore();

    std::vector<MsgEntry> OutFilteredMsgs;

    //Results.FilterByEntryName("EntryName", FilteredMsgs);
    Results.FilterByMsgType(MsgType::Desync, OutFilteredMsgs);

    std::cout << "Filtered entities:" << std::endl;
    for (auto const& Msg : OutFilteredMsgs)
    {
        std::cout << Msg.Msg;
    }

    std::cout << std::endl << "Press Enter..." << std::endl;
    std::cin.ignore();

    return 0;
}
