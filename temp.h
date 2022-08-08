#include <ErriezOregonTHN128Esp32Receive.h> // forked from for esp8266 https://github.com/Erriez/ErriezOregonTHN128

struct SensorData {
  uint32_t rxCount;
  char temp[10];
  int channel;
  int lowBattery;  
};

void printReceivedData(SensorData *sensorData)
{
    char msg[80];
    snprintf_P(msg, sizeof(msg), PSTR("RX %lu, Channel %d, Temp: %s, Low batt: %d"),
               sensorData->rxCount, sensorData->channel, sensorData->temp, sensorData->lowBattery);
    if (SERIAL) Serial.println(msg);
}

SensorData formatSensorData(OregonTHN128Data_t *rawData)
{
    SensorData sensorData;
    bool negativeTemperature = false;
    int16_t tempAbs;
    char temp[10];

    // Convert to absolute temperature
    tempAbs = rawData->temperature;
    if (tempAbs < 0) {
        negativeTemperature = true;
        tempAbs *= -1;
    }

    snprintf_P(sensorData.temp, sizeof(sensorData.temp), PSTR("%s%d.%d"), (negativeTemperature ? "-" : ""), (tempAbs / 10), (tempAbs % 10));

    sensorData.channel = rawData->channel;
    sensorData.lowBattery = rawData->lowBattery;
    return sensorData;
}
