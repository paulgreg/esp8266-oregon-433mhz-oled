#include <Arduino.h>

#define SERIAL true

#define SECOND 1e6
#define MINUTE 60e6

#define DEFAULT_SLEEP_TIME 30 * MINUTE
#define SHORT_SLEEP_TIME 5 * MINUTE

#include "parameters.h"
#include "temp.h"
#include "network.h"
#include "display.h"

static uint32_t rxCount = 0;
OregonTHN128Data_t rawData;
SensorData sensorData;

void setup()
{
  if (SERIAL) {
    Serial.begin(115200);
    Serial.setTimeout(2000);
    while(!Serial) {}
    delay(250);
    Serial.println(F("\nOregon THN128 433MHz temperature receiver"));
  }
  
  delay(250);
  OregonTHN128_RxBegin(RF_RX_PIN);

  setupScreen();
  delay(250);
}

void loop() {
  boolean longSleep = false;
  
  if (OregonTHN128_Available()) {
      OregonTHN128_Read(&rawData);

      sensorData = formatSensorData(&rawData);
      sensorData.rxCount = rxCount++;

      if (SERIAL) printReceivedData(&sensorData);
      displayData(&sensorData);

      if (connectToWifi()) {
        if (postData(&sensorData)) {
          longSleep = true;
        }
        // disconnectWifi();
      }

      if (SERIAL) {
        Serial.println(F("Will now sleep"));
        Serial.flush();
        delay(100);
      }

      // You need to connect D0 to RST after flash in order to wakeup
      // https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
      ESP.deepSleep(longSleep ? DEFAULT_SLEEP_TIME : SHORT_SLEEP_TIME);

      // OregonTHN128_RxEnable(); // If we wan’t to ask for another temp
  }
}
