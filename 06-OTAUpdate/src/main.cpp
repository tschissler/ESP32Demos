#include <Arduino.h>
#include <ESP32httpUpdate.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>  

#define onboardLED 2
#define redLED 27
#define greenLED 14
#define blueLED 16

int i = 0;

void updateFromBLOB(String url) {
    Serial.println("////////// Checking for Firmeware update from BLOB //////////");
    t_httpUpdate_return ret = ESPhttpUpdate.update(url);
    
    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
    }
    Serial.println("////////////////////////////////////////////////////////////");
    Serial.println();
}

void setup() {
  Serial.begin(115200);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");

  Serial.println("Successfully connected to WiFi");
}

void loop() {
  digitalWrite(redLED, HIGH);
  delay(100);
  digitalWrite(redLED, LOW);
  delay(500);
  if (i>20) {
    	updateFromBLOB("<blobstoragefileURL>");
  }
  i++;
}