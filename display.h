#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// https://chewett.co.uk/blog/2543/i2c-connection-pins-on-the-wemos-d1-mini-esp8266/
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an esp32:       SDA (default is GPIO 21) & SCL (default is GPIO 22) 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupScreen() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.cp437(true);
}

int loopIdx = 0;

void displayTemperature(SensorData *sensorData) {
  char temp[20], rx[40], ch[40];

  snprintf_P(temp, sizeof(temp), PSTR("T: %s"), sensorData->temp);
  snprintf_P(rx, sizeof(rx), PSTR("RX:%lu"), sensorData->rxCount);
  snprintf_P(ch, sizeof(ch), PSTR("Ch. %d - Low batt: %d"), sensorData->channel, sensorData->lowBattery);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(temp);
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.print(rx);
  display.setCursor(0, 40);
  display.print(ch);
}


void displayData(SensorData *sensorData) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  displayTemperature(sensorData);
  display.display();
}
