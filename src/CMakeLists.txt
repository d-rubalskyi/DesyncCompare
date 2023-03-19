cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 17)

project(LogCompare)

add_executable(LogCompare main.cpp 
    main.cpp 
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
