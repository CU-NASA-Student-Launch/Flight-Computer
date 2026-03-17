#include <Arduino.h>
#include <array>
#include "LittleFS.h"
#include <atomic>
#include <SensorData.hpp>

#ifndef FS_WRITER_HPP
#define FS_WRITER_HPP

class FsWriter {
    public:
        FsWriter();

        ~FsWriter();

        void initiate();

        void flush();

        void store(SensorData &record);

        void streamFSData();

        void checkForFile();

        void closeFile();
        
    private:
        std::array<SensorData, 5000> sensorDataBuff;
        int currBuffLoc = 0;
        File logFile;
};

#endif // FS_WRITER
