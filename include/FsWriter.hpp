#include <Arduino.h>
#include <array>
#include "SensorData.hpp"

class FsWriter {
    public:
        FsWriter();

        ~FsWriter();

        void flush();

        void store(SensorData &record);

        void streamFSData();
        
    private:
        std::array<SensorData, 60> sensorDataBuff;
        int currBuffLoc = 0;
};