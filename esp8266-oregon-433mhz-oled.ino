#include <Arduino.h>

#include "parameters.h"
#include "temp.h"
#include "network.h"
#include "display.h"

#define SECOND 1e6
#define MINUTE 60 * SECOND

static uint32_t rxCount = 0;
OregonTHN128Data_t rawData;
SensorData sensorData;

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(2000);
    while(!Serial) {}
    delay(250);
    Serial.println(F("\nOregon THN128 433MHz temperature receiver"));
    
    delay(250);
    OregonTHN128_RxBegin(RF_RX_PIN);

    delay(250);
    setupScreen();
}

void loop()
{
    if (OregonTHN128_Available()) {
        OregonTHN128_Read(&rawData);

        sensorData = formatSensorData(&rawData);
        sensorData.rxCount = ++rxCount;

        printReceivedData(&sensorData);
        
        displayData(&sensorData);

        connectToWifi();
        postData(&sensorData);
        // disconnectWifi();

        Serial.println(F("Sleep"));
        Serial.flush();
        delay(100);
        // You need to connect D0 to RST after flash in order to wakeup
        // https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
        ESP.deepSleep(30e6);

        // OregonTHN128_RxEnable(); // If we wan’t to ask for another temp
    }
}
