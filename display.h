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
    if (SERIAL) Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.cp437(true);
  display.setTextColor(WHITE);
}

void testCharacters() {
  for (int i=0; i< 255; i++) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print(i);
    display.setCursor(0, 30);
    display.write((char) i);    
    display.display();
    delay(500);
  }
}

void displayData(SensorData *sensorData, String msg) {
  char ch[40];
  snprintf_P(ch, sizeof(ch), sensorData->lowBattery == 0 ? PSTR("Ch. %d") : PSTR("Ch. %d - Low battery"), sensorData->channel);

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 10);

  display.print("T");
  display.setCursor(12, 5);
  display.write((char) 9);
  display.setCursor(20, 10);
  display.print(":");
  display.print(sensorData->temp);
  
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(ch);
  display.setCursor(0, 55);
  display.print(msg);
  display.display();
}

void displaySearching() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Searching");
  display.display();
}
