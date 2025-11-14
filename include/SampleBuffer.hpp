#include <Arduino.h>
#include "Record.hpp"

class SampleBuffer {
    public:
        SampleBuffer();

        ~SampleBuffer();

        void flush();

        void store(Record &record);

        bool full();

        void streamFSData();
        
    private:
        int BUFFER_SIZE;
        Record* buffer; // pointer to buffer array
        int remainingCapacity;
};