#include <cstdio>

#include "Cluster.h"

int main()
{
    Cluster NodeCluster;
    ComparisonResult Results;

    NodeCluster.LoadNodeData();
    NodeCluster.CompareNodeData(Results);

    Results.Print();

    std::vector<MsgEntry> OutDesyncMsgs;
    std::vector<MsgEntry> OutUniqueDesyncMsgs;
    
    //Results.FilterByEntryName("EntryName", FilteredMsgs);

    Results.FilterByMsgType(MsgType::Desync, OutDesyncMsgs);
    Results.FilterUniqueMsgs(OutDesyncMsgs, OutUniqueDesyncMsgs);

    if (!OutUniqueDesyncMsgs.empty()) 
    {
        std::cout << std::endl << "Unique " << RedColor << "[Desync]"
            << WhiteColor << " entries:" << std::endl;

        for (auto const& Msg : OutUniqueDesyncMsgs)
        {
            std::cout << RedColor << "  [Desync]"
                << YellowColor << "Frame[" << Msg.FrameIdx << "]"
                << WhiteColor << Msg.Entry << std::endl;
        }
    }

    std::cout << std::endl << "Press Enter..." << std::endl;
    std::cin.ignore();

    return 0;
}
