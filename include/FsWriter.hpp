#include <Arduino.h>
#include <array>
#include "LittleFS.h"
#include <atomic>
#include <SensorData.hpp>
#include <SpeakerControl.hpp>

#include <ServoControl.hpp>

#ifndef FS_WRITER_HPP
#define FS_WRITER_HPP

class FsWriter
{
public:
    FsWriter();

    ~FsWriter();

    void initiate();

    void flush();

    void store(SensorData &record, ServoControl &servo);

    void streamFSData();

    void checkForFile(SpeakerControl &speaker);

private:
    std::array<SensorData, 5000> sensorDataBuff;
    int currBuffLoc = 0;
    File logFile;
};

#endif // FS_WRITER
