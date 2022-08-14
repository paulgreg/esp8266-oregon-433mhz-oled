#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
HTTPClient http;

boolean connectToWifi() {
  #if SERIAL 
  Serial.print("\nconnecting to ");
  Serial.println(WIFI_SSID);
  #endif
  
  WiFi.forceSleepWake();
  delay(1);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned int retries = 100;
  while (WiFi.status() != WL_CONNECTED && (retries-- > 0)) {
    delay(200);
    #if SERIAL 
    Serial.print(".");
    #endif
  }
  if (WiFi.status() != WL_CONNECTED) {
    #if SERIAL 
    Serial.println("\nWifi connection failed");
    #endif
    return false;
  }
  #if SERIAL 
  Serial.println("");
  Serial.println("wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  #endif
  return true;
}

/* trying to stop wifi cause that exception :
Exception (0):
epc1=0x40201471 epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000

>>>stack>>>

ctx: cont
sp: 3ffffdc0 end: 3fffffc0 offset: 0190
3fffff50:  3fffdad0 00000001 3ffee9cf 40201471  
3fffff60:  3fffdad0 00000001 3ffee8c0 40201c20  
3fffff70:  00000000 6e657320 00000000 3ffeec8c  
3fffff80:  00000000 372e3732 00000000 40100350  
3fffff90:  00000001 00000000 3ffeec78 40100371  
3fffffa0:  feefeffe 00000000 3ffeec78 4020732c  
3fffffb0:  feefeffe feefeffe 3ffe85e0 401012cd  
<<<stack<<<
*/
boolean disconnectWifi() {
  #if SERIAL 
  Serial.println("\nWifi disconnected");
  #endif
  WiFi.disconnect(); 
  delay(1000);
  #if SERIAL 
  Serial.println("\nWifi off");
  #endif
  WiFi.mode(WIFI_OFF);
  #if SERIAL 
  Serial.println("\nWifi forceSleepBegin");
  #endif
  WiFi.forceSleepBegin();
  delay(100);
}

boolean postData(SensorData *sensorData) {
  boolean success = false;
  WiFiClientSecure client;
  client.setInsecure();

  #if SERIAL 
  Serial.println("POSTing data");
  #endif
  
  if (!client.connect(HOST, 443)) {
    #if SERIAL 
    Serial.println("Connection failed");
    #endif
    return false;
  }

  String payload = String("{") + 
  "\"temp\":\"" + sensorData->temp + "\"," +
  "\"ch\":" + sensorData->channel + "," + 
  "\"lowbat\":"+ sensorData->lowBattery + 
  "}";
  
  String request = String("POST ") + URL + 
  " HTTP/1.1\r\n" + 
  "Host: " + HOST + "\r\n" +
  "Authorization: " + AUTHORIZATION + "\r\n" +
  "User-Agent: Esp8266TemperatureProxy\r\n" + 
  "Content-Type: application/json\r\n" + 
  "Content-Length: " + payload.length() + "\r\n" + 
  "Connection: close\r\n" +
  "\r\n" +
  payload;

  #if SERIAL 
  Serial.println(request);
  #endif
  client.print(request);

  unsigned long timeout = millis();
  while (client.available() == 0) {
    delay(1);
    if (millis() - timeout > REQUEST_TIMEOUT) {
      #if SERIAL
      Serial.println("client Timeout !");
      #endif
      client.stop();
      return false;
    }
  }
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    #if SERIAL 
    Serial.print(line);
    #endif
  }
  
  #if SERIAL 
  Serial.println("Closing connection");
  #endif
  client.stop();
  
  return true;
}
