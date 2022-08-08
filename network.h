#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
HTTPClient http;

boolean connectToWifi() {
  if (SERIAL) {
    Serial.print("\nconnecting to ");
    Serial.println(WIFI_SSID);
  }

  WiFi.forceSleepWake();
  delay(1);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned int retries = 100;
  while (WiFi.status() != WL_CONNECTED && (retries-- > 0)) {
    delay(200);
    if (SERIAL) Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    if (SERIAL) Serial.println("\nWifi connection failed");
    return false;
  }
  if (SERIAL) {
    Serial.println("");
    Serial.println("wifi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
  }
  return true;
}

boolean disconnectWifi() {
  if (SERIAL) Serial.println("\nWifi sleep");
  WiFi.disconnect();
  delay(1); 
  WiFi.forceSleepBegin();
  delay(1); //For some reason the modem won't go to sleep unless you do a delay
}


boolean postData(SensorData *sensorData) {
  boolean success = false;
  WiFiClientSecure client;
  client.setInsecure();

  if (SERIAL) Serial.println("POSTing data");
  
  if (!client.connect(HOST, 443)) {
    if (SERIAL) Serial.println("Connection failed");
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

  if (SERIAL) Serial.println(request);
  client.print(request);

  unsigned long timeout = millis();
  while (client.available() == 0) {
    delay(1);
    if (millis() - timeout > REQUEST_TIMEOUT) {
      if (SERIAL) Serial.println("client Timeout !");
      client.stop();
      return false;
    }
  }
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    if (SERIAL) Serial.print(line);
  }
  
  if (SERIAL) Serial.println("Closing connection");
  client.stop();
  
  return true;
}
