#include <Arduino.h>
#include "Record.hpp"

class FsWriter {
    public:
        FsWriter();

        ~FsWriter();

        void flush();

        void store(Record &record);

        bool full();

        void streamFSData();
        
    private:
        int BUFFER_SIZE;
        Record* buffer; // pointer to buffer array
        int remainingCapacity;
};