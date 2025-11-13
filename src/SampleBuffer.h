#pragma once
#include <Arduino.h>

class SampleBuffer {
    public:
        SampleBuffer();

        ~SampleBuffer();

        void Flush();

        void Collect();

        bool Full();

        void StreamFSData();
        
    private:
        struct Record {
            uint32_t t_ms; // time in milliseconds
            float x; // data value
        };
        Record record;
        int BUFFER_SIZE;
        Record* buffer; // pointer to buffer array
        int remainingCapacity;
};