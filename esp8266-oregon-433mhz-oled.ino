#include <Arduino.h>

#define SERIAL false

#define SECOND 1000

#define DEFAULT_SLEEP_TIME  900000000 // 15 * 60 * 1e6
#define SHORT_SLEEP_TIME    300000000 //  5 * 60 * 1e6

#include "parameters.h"
#include "temp.h"
#include "network.h"
#include "display.h"

static uint32_t rxCount = 0;
OregonTHN128Data_t rawData;
SensorData sensorData;

void setup() {
  if (SERIAL) {
    Serial.begin(115200);
    Serial.setTimeout(2000);
    while(!Serial) {}
    Serial.println(F("\nOregon THN128 433MHz temperature receiver & relay"));
  }

  pinMode(D0, WAKEUP_PULLUP);
  
  delay(250);
  OregonTHN128_RxBegin(RF_RX_PIN);

  setupScreen();
  delay(500);
  displaySearching();
}

void loop() {
  // testCharacters();
  boolean longSleep = false;
  
  if (OregonTHN128_Available()) {
      OregonTHN128_Read(&rawData);

      sensorData = formatSensorData(&rawData);
      sensorData.rxCount = rxCount++;

      if (SERIAL) printReceivedData(&sensorData);
      displayData(&sensorData, "");

      delay(SECOND);
      displayData(&sensorData, "Connecting");
      if (connectToWifi()) {
        delay(SECOND);
        displayData(&sensorData, "Sending data");
        if (postData(&sensorData)) {
          longSleep = true;
          displayData(&sensorData, "Data sent");
          delay(SECOND);
        } else {
          displayData(&sensorData, "Posting error");
        }
        // disconnectWifi();
        // displayData(&sensorData, "Disconnected");
        // delay(5 * SECOND);
      } else {
        displayData(&sensorData, "Connection error");
      }

      delay(2 * SECOND);
      displayData(&sensorData, "Sleeping");

      if (SERIAL) {
        Serial.println(F("Will now sleep"));
        Serial.flush();
        delay(100);
      }

      // You need to connect D0 to RST after flash in order to wakeup
      // https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
      ESP.deepSleep(longSleep ? DEFAULT_SLEEP_TIME : SHORT_SLEEP_TIME, WAKE_RF_DEFAULT);

      // OregonTHN128_RxEnable(); // If we wan’t to ask for another temp
  }
}
