#include "LogReader.h"

#include <iostream>

bool LogReader::Open(std::string const& FileName)
{
    FileStream.open(FileName);

    if (!FileStream.is_open())
    {
        return false;
    }

    return true;
}

bool LogReader::ReadNextLine(EntryData& Data, int& OutFrame)
{
    getline(FileStream, Line);
    
    LineNumber++;

    static std::string LineMarker = "(DataIntegrityTest)";
    int LabelIndex = static_cast<int>(Line.find(LineMarker));

    if (LabelIndex < 0)
    {
        return false;
    }

    if (!Data.ParseLine(Line, OutFrame))
    {
        return false;
    }

    return true;
}

bool LogReader::ReadNextFrame(FrameData& FrameData, int FrameCounter)
{
    if (IsFileReadFinished())
    {
        return false;
    }

    FrameData.FrameNumber = -1;
    FrameData.Data.clear();

    while (!FileStream.eof())
    {
        EntryData Data(LineNumber);

        int OutFrame = -1;

        if (ReadNextLine(Data, OutFrame))
        {
            // Add LineData from previous Reads
            if (bAddPreviousEntryData)
            {
                std::size_t ActorHash = std::hash<std::string>{}(PreviousEntryData.GetName());

                FrameData.Data[ActorHash].push_back(PreviousEntryData);
                FrameData.FrameNumber = LastFrameNumber;

                bAddPreviousEntryData = false;

                // Previous Read contained only one entry - save current and exit
                if (OutFrame != LastFrameNumber)
                {
                    PreviousEntryData = Data;
                    LastFrameNumber = OutFrame;

                    bAddPreviousEntryData = true;
                    break;
                }
            }

            // Init FrameNumber
            if (FrameData.FrameNumber == -1)
            {
                FrameData.FrameNumber = OutFrame;
            }

            // Add to Entry to the FrameData
            if (FrameData.FrameNumber == OutFrame)
            {
                std::size_t ActorHash = std::hash<std::string>{}(Data.GetName());
                FrameData.Data[ActorHash].push_back(Data);
            }

            // Reached next Frame - cache and add on next invocation
            if (FrameData.FrameNumber != OutFrame)
            {
                PreviousEntryData = Data;
                LastFrameNumber = OutFrame;

                bAddPreviousEntryData = true;
                break;
            }
        }
    }

    return (FrameData.FrameNumber != -1);
}

void LogReader::Close()
{
    if (FileStream.is_open())
    {
        FileStream.close();
    }
}
