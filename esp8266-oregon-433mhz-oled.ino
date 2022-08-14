#include <Arduino.h>

#define SECOND 1000

#define SLEEP_TIME 60e6 // 900000000 // 15 * 60 * 1e6

#include "parameters.h"
#include "temp.h"
#include "network.h"
#if SCREEN
#include "display.h"
#endif

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

  pinMode(D0, WAKEUP_PULLUP);
  
  delay(250);
  OregonTHN128_RxBegin(RF_RX_PIN);

  #if SCREEN
  setupScreen();
  delay(500);
  displaySearching();
  #endif

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
      #if SCREEN
      displayData(&sensorData, "");
      delay(SECOND);
      #endif

      #if SCREEN
      displayData(&sensorData, "Connecting");
      #endif
      if (connectToWifi()) {
        delay(SECOND);
        #if SCREEN
        displayData(&sensorData, "Sending data");
        #endif
        if (postData(&sensorData)) {
          #if SCREEN
          displayData(&sensorData, "Data sent");
          #endif
          delay(SECOND);
        } else {
          #if SCREEN
          displayData(&sensorData, "Posting error");
          #endif
        }
        // disconnectWifi();
        // displayData(&sensorData, "Disconnected");
        // delay(5 * SECOND);
      } else {
        #if SCREEN
        displayData(&sensorData, "Connection error");
        #endif
      }

      #if SCREEN
      delay(2 * SECOND);
      displayData(&sensorData, "Sleeping");
      // delay(SECOND);
      // sleepScreen();
      // delay(500);
      #endif

      #if SERIAL
      Serial.println(F("Will now sleep"));
      Serial.flush();
      delay(100);
      #endif
      
      // You need to connect D0 to RST after flash in order to wakeup
      // https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
      // I’ve got issue with deepSleep, cf https://github.com/esp8266/Arduino/issues/6007
      ESP.deepSleep(SLEEP_TIME, WAKE_RF_DEFAULT);
      
      rxCount++;
      OregonTHN128_RxEnable();
  }
}
