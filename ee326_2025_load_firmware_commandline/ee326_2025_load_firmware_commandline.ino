#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "esp_eap_client.h"

/****************************
*** Choose a network type ***
****************************/
// #define WIFI_REGULAR
// #define WIFI_OPEN
#define WIFI_NU

/****************************************
*** Modify credentials as appropriate ***
****************************************/
#if defined(WIFI_REGULAR)
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";
#elif defined(WIFI_OPEN)
const char* ssid = "REPLACE_WITH_YOUR_SSID";
#elif defined(WIFI_NU)
const char* ssid = "eduroam";
const char* eap_anonymous_id = "anonymous@northwestern.edu";
const char* eap_id = "ahh8210@northwestern.edu"; // replace "YOUR_NETID" with your net ID
const char* eap_password = "Kasun2001@96"; // replace with your password
#endif

const char* ota_name = "esd1";
const char* ota_password = "esd1";

void start_ota() { // Start the OTA service
  ArduinoOTA.setHostname(ota_name);
  ArduinoOTA.setPassword(ota_password);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready\r\n");
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("");

  Serial.println(WiFi.macAddress());
  uint8_t mac[6];
  WiFi.macAddress(mac);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
#if defined(WIFI_REGULAR)
  Serial.println("Connecting to regular network");
  WiFi.begin(ssid, password);
#elif defined(WIFI_OPEN)
  Serial.println("Connecting to open network");
  WiFi.begin(ssid);
#elif defined(WIFI_NU)
  Serial.println("Connecting to WPA2 Enterprise network");
  esp_eap_client_set_identity((uint8_t*) eap_anonymous_id, strlen(eap_anonymous_id));
  esp_eap_client_set_username((uint8_t*) eap_id, strlen(eap_id));
  esp_eap_client_set_password((uint8_t*) eap_password, strlen(eap_password));
  esp_wifi_sta_enterprise_enable();
  WiFi.begin(ssid);
#endif

  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  start_ota();
}

void loop(void) {
  ArduinoOTA.handle();
  vTaskDelay(2);
}