#include <FsWriter.hpp>

FsWriter::FsWriter()
{
}

FsWriter::~FsWriter()
{
}

void FsWriter::checkForFile(SpeakerControl &speaker)
{
    LittleFS.begin();

    // Check if file exists. If so, enter loop that talks to python script.
    if (LittleFS.exists("/flight.bin"))
    {
        speaker.fileCheckBeep();
        this->streamFSData();
    }
}

void FsWriter::initiate()
{
    logFile = LittleFS.open("/flight.bin", "w");
    logFile.close();
}

void FsWriter::flush()
{
    logFile = LittleFS.open("/flight.bin", "a");

    if (!logFile)
        return;

    int count = currBuffLoc;
    if (count == 0)
        return;

    // Write raw SensorData structs directly
    logFile.write(
        (uint8_t *)&sensorDataBuff[0],
        count * sizeof(SensorData));

    currBuffLoc = 0;

    logFile.close(); // Flushes LFS buffer
}

void FsWriter::store(SensorData &record, ServoControl &servo)
{
    // Check if the buffer is full before writing
    // The buffer is used to write data in large chunks. This saves time as writing to
    // flash is a very slow operation and flash wears out over time.
    if (currBuffLoc == sensorDataBuff.size() - 1)
    {
        servo.adjustAngle(0);
        tone(22, 1500);
        this->flush();
        currBuffLoc = 0;
    }

    sensorDataBuff[currBuffLoc] = record;
    currBuffLoc++;
}

// This function talks to a python script that coordinates the transmission of the data.
void FsWriter::streamFSData()
{
    while (1)
    {
        while (!Serial)
            delay(10);

        String word = Serial.readStringUntil('\n');

        if (word == "GET")
        {
            Serial.println("ACK: GET");

            if (!LittleFS.exists("/flight.bin"))
            {
                Serial.println("FILE_NOT_FOUND");
                continue;
            }

            File f = LittleFS.open("/flight.bin", "r");
            if (!f)
            {
                Serial.println("CANNOT_OPEN_FILE");
                continue;
            }

            Serial.println("BEGIN_FILE");

            Serial.println("msTime,accelX(m/s^2),accelY(m/s^2),accelZ(m/s^2),gyroX(rad/sec),gyroY(rad/sec),gyroZ(rad/sec),pressure (Pa),altitude (m),angle (degrees),gps_alt (m),gps_lat (deg),gps_lon (deg)");

            SensorData record;

            while (f.read((uint8_t *)&record, sizeof(SensorData)) == sizeof(SensorData))
            {
                Serial.printf("%lu,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                              record.t_ms,
                              record.accelX,
                              record.accelY,
                              record.accelZ,
                              record.gyroX,
                              record.gyroY,
                              record.gyroZ,
                              record.pressure,
                              record.altitude,
                              record.angle,
                              record.gps_alt,
                              record.gps_lat,
                              record.gps_lon);
            }

            f.close();

            Serial.println("END_FILE");

            Serial.setTimeout(300000); // 5 minutes
            String response = Serial.readStringUntil('\n');

            if (response == "ACK")
            {
                String decision = Serial.readStringUntil('\n');

                if (decision == "YES")
                {
                    if (!LittleFS.remove("/flight.bin"))
                    {
                        Serial.println("Unable to remove old flight.bin.");
                    }
                    else
                    {
                        Serial.println("Removed flight.bin from flash");
                    }
                }
                else if (decision == "NO")
                {
                    Serial.println("Keeping flight.bin on flash.");
                }
            }
        }
    }
}
