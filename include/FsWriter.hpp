#include <Arduino.h>
#include <array>
#include "SensorData.hpp"

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
        
    private:
        std::array<SensorData, 5000> sensorDataBuff;
        int currBuffLoc = 0;
};

#endif // FS_WRITER
