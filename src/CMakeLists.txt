cmake_minimum_required(VERSION 3.16)

project(DesyncCompare)

set (CMAKE_CXX_STANDARD 17)

add_library(DesyncCompare STATIC
    EntryData.cpp
    EntryData.h
    Cluster.cpp
    Cluster.h
    LogReader.cpp
    LogReader.h
    ComparisonResult.cpp
    ComparisonResult.h
    ConsoleColors.h
)
