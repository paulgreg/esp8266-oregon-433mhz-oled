#include <Arduino.h>

#define SECOND 1000

#include "parameters.h"
#include "temp.h"
#include "network.h"
#include "display.h"

static uint32_t rxCount = 0;
OregonTHN128Data_t rawData;
SensorData sensorData;

void setup() {
  #if SERIAL
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while(!Serial) {}
  Serial.println(F("\nOregon THN128 433MHz temperature receiver & relay"));
  #endif
  
  delay(250);
  OregonTHN128_RxBegin(RF_RX_PIN);

  setupScreen();
  delay(500);
  displaySearching();

  WiFi.setAutoReconnect(false);
  WiFi.persistent(false);

}

void loop() {
  // testCharacters();

  if (OregonTHN128_Available()) {
      OregonTHN128_Read(&rawData);

      sensorData = formatSensorData(&rawData);
      sensorData.rxCount = rxCount;

      #if SERIAL
      printReceivedData(&sensorData);
      #endif
      displayData(&sensorData, "");
      delay(SECOND);

      if ((rxCount % 100) == 0) {
        displayData(&sensorData, "Connecting");
        if (connectToWifi()) {
          delay(SECOND);
          displayData(&sensorData, "Sending data");
          if (postData(&sensorData)) {
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
      }
      
      rxCount++;
      OregonTHN128_RxEnable();
  }
}
