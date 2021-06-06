// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. 

#include "AzureIotHub.h"
#include "Esp32MQTTClient.h"
#include <ESP32httpUpdate.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>  

#define onboardLED 2
#define redLED 27
#define greenLED 14
#define blueLED 16

#define redChannel 0
#define greenChannel 1
#define blueChannel 2

int redBrightness = 0;
int greenBrightness = 100;
int blueBrightness = 0;

#define pwm_freq 5000
#define pwm_resolution 8

#define INTERVAL 30000
#define DEVICE_ID "DemoDevice"
#define MESSAGE_MAX_LEN 256
#define CURRENT_VERSION "<CurrentFirmwareVersion>"
#define BLOB_STORAGE_ROOT "https://iotdemotschissler.blob.core.windows.net/firmwareupdates"

#define DHTPIN 17
// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT_Unified dht(DHTPIN, DHTTYPE);

// Please input the SSID and password of WiFi
const char* ssid     = "WLAN_M1_Guest";
const char* password = "NichtUebertreiben";

/*String containing Hostname, Device Id & Device Key in the format:                         */
static const char* connectionString = "HostName=IoTHub-tschissler.azure-devices.net;DeviceId=DemoDevice;SharedAccessKey=GsqqUNQi8Wn35W2jYeW8yOSvr9sS/69xs/d3Ava+ok8=";

const char *messageData = "{\"deviceId\":\"%s\", \"messageId\":%d, \"Temperature\":%f, \"Humidity\":%f}";

int messageCount = 1;
static bool hasWifi = false;
static bool messageSending = true;
static uint64_t send_interval_ms;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities

static void updateFromBLOB(String url) {
    Serial.print("Downloading new firmware from: ");
    Serial.println(url);
    
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
                Serial.println("Update done");
            break;
    }
    Serial.println();
}

static void InitWifi()
{
  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // WiFiManager wifiManager;
  // wifiManager.autoConnect("AutoConnectAP");

  Serial.println("Successfully connected to WiFi");
  hasWifi = true;
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    Serial.println("Send Confirmation Callback finished.");
  }
}

static void MessageCallback(const char* payLoad, int size)
{
  Serial.println("Message callback:");
  Serial.println(payLoad);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);

  temp[size] = '\0';
  // Display Twin message.
  Serial.println(temp);
  DynamicJsonDocument doc(5000);
  deserializeJson(doc, temp);

  redBrightness =  doc["desired"]["red"];
  blueBrightness = doc["desired"]["blue"];
  greenBrightness = doc["desired"]["green"];

  Serial.print("blue: ");
  Serial.println(blueBrightness);

  Serial.println("Checking desired firmware version to decide if update is needed");
  Serial.print("Current firmware version: ");
  Serial.println(CURRENT_VERSION);
  Serial.print("Desired firmware version: ");
  const char* desiredFirmwareVersion = doc["desired"]["firmwareversion"];
  Serial.println(desiredFirmwareVersion);

  if (strcmp(desiredFirmwareVersion, CURRENT_VERSION) != 0) {
    Serial.println();
    Serial.println("Starting Firmware OTA update ...");
    char url [1000];
    sprintf(url, "%s/%s/firmware.bin", BLOB_STORAGE_ROOT, desiredFirmwareVersion); 
    updateFromBLOB(url);
  }

  free(temp);
}

static int  DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  LogInfo("Try to invoke method %s", methodName);
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    LogInfo("Start sending temperature and humidity data");
    messageSending = true;
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    LogInfo("Stop sending temperature and humidity data");
    messageSending = false;
  }
  else
  {
    LogInfo("No method %s found", methodName);
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}

void setLEDOn() {
  ledcWrite(redChannel, redBrightness);
  ledcWrite(greenChannel, greenBrightness);
  ledcWrite(blueChannel, blueBrightness);
}

void setLEDOff() {
  ledcWrite(redChannel, 0);
  ledcWrite(greenChannel, 0);
  ledcWrite(blueChannel, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino sketch
void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println("##############################################################");
  Serial.println("### ESP32 Azure OTA Update Demo");
  Serial.print("### Current Version: ");
  Serial.println(CURRENT_VERSION);
  Serial.println("##############################################################");
  Serial.println("ESP32 Device");
  Serial.println("Initializing...");

  // Initialize the WiFi module
  Serial.println(" > WiFi");
  hasWifi = false;
  InitWifi();
  if (!hasWifi)
  {
    return;
  }
  randomSeed(analogRead(0));

  Serial.println(" > IoT Hub");
  Esp32MQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "GetStarted");
  Esp32MQTTClient_Init((const uint8_t*)connectionString, true);

  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);

  send_interval_ms = millis();

  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.

  Serial.println(F("Initializing LED PWM"));
  ledcSetup(redChannel, pwm_freq, pwm_resolution);
  ledcSetup(greenChannel, pwm_freq, pwm_resolution);
  ledcSetup(blueChannel, pwm_freq, pwm_resolution);

  ledcAttachPin(redLED, redChannel);  
  ledcAttachPin(greenLED, greenChannel);  
  ledcAttachPin(blueLED, blueChannel);  
}

void loop()
{
  if (hasWifi)
  {
    if (messageSending && 
        (int)(millis() - send_interval_ms) >= INTERVAL)
    {
      // Send teperature data
      char messagePayload[MESSAGE_MAX_LEN];

      sensors_event_t event;
      dht.temperature().getEvent(&event);
      if (isnan(event.temperature)) {
        Serial.println(F("Error reading temperature!"));
      }
      else {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
      }
      float temperature = event.temperature;

      // Get humidity event and print its value.
      dht.humidity().getEvent(&event);
      if (isnan(event.relative_humidity)) {
        Serial.println(F("Error reading humidity!"));
      }
      else {
        Serial.print(F("Humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
      }
      float humidity = event.relative_humidity;

      snprintf(messagePayload,MESSAGE_MAX_LEN, messageData, DEVICE_ID, messageCount++, temperature, humidity);
      Serial.println(messagePayload);
      EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
      Esp32MQTTClient_Event_AddProp(message, "temperatureAlert", "true");
      Esp32MQTTClient_SendEventInstance(message);
      
      send_interval_ms = millis();
    }
    else
    {
      Esp32MQTTClient_Check();
    }
  }

  setLEDOn();
  delay(100);
  // setLEDOff();
  // delay(100);
  // setLEDOn();
  // delay(100);
  // setLEDOff();
  // delay(500);
}