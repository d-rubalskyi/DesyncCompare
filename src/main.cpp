#include <cstdio>

#include "Cluster.h"

int main()
{
    Cluster NodeCluster;
    FrameComparisonData Results;

    NodeCluster.ExtractNodeData();
    NodeCluster.CompareNodeData(Results);

    Results.Print();

    std::cout << std::endl << "Press Enter..." << std::endl;
    std::cin.ignore();

    return 0;
}
