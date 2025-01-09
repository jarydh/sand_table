// Import required libraries
#include "WiFi.h"
#include "WifiManager.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

#include "web_server.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String &var)
{
  return String();
}

void init_webserver()
{
  Serial.println("initting websierver");
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;

  bool res;
  res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

  if (!res)
  {
    Serial.println("Failed to connect");
  }
  else
  {
    Serial.println("Connected to wifi");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false); });

  // Modified POST handler with body parsing
  server.on("/api/draw", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    // This handler will be called when the body is complete
    Serial.println("Draw baby boy");
    request->send(200); }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
    // Handler for body data
    // Create a buffer to store the body data as a string
    char *buf = (char*)malloc(len + 1);
    memcpy(buf, data, len);
    buf[len] = '\0';
    
    // Print the received data
    Serial.print("Received body data: ");
    Serial.println(buf);
    Serial.print("Chunk index: ");
    Serial.println(index);
    Serial.print("Total length: ");
    Serial.println(total);
    
    free(buf); });

  // Start server
  server.begin();
}