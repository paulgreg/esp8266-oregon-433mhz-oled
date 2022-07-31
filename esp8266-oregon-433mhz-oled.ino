#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ErriezOregonTHN128Esp32Receive.h> // Lib from https://github.com/Erriez/ErriezOregonTHN128
#include "parameters.h"
#include "temp.h"
#include "display.h"

static uint32_t rxCount = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("\nOregon THN128 433MHz temperature receive"));
    delay(100);

    OregonTHN128_RxBegin(RF_RX_PIN);

    delay(100);
    setupScreen();
}

void loop()
{
    OregonTHN128Data_t rawData;
    SensorData sensorData;

    if (OregonTHN128_Available()) {      
        rxCount++;
        OregonTHN128_Read(&rawData);

        sensorData = formatSensorData(&rawData);
        printReceivedData(&sensorData);

        displayData(&sensorData);
        
        // Enable receive
        OregonTHN128_RxEnable();
    }
}
