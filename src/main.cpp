#include "SPIFFS.h"
// #include "stepper_control.h"
#include "WiFi.h"
#include "WifiManager.h"
#include "ESPAsyncWebServer.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

#define LED_PIN 22
#define STEP_PIN_1 12
#define DIR_PIN_1 14
#define STEP_PIN_2 26
#define DIR_PIN_2 27
#define SERIAL_PORT 115200

#define STEPS_PER_REV 200
#define STEPPER_GEAR_RATIO 1.8

#define START_POSITION -1, 0
#define FILE_NAME "/positions.in"

void setup()
{
  Serial.begin(SERIAL_PORT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

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
  Serial.printf("http://%s\n", WiFi.localIP().toString());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false); 
            Serial.println("served '/index.html'"); });

  // Modified POST handler with body parsing
  server.on("/api/draw", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              // This handler will be called when the body is complete
              request->send(200);
              //
            },
            NULL,
            //
            // Handler for body data
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
              Serial.println("Handling body");
              String body = String((char *)data).substring(0, len);
              double x, y;
              int i = 0, j = 0;
              char ch;
              while (j <= body.length())
              {
                ch = body.charAt(j);
                if (ch == ',')
                {
                  x = body.substring(i, j).toDouble();
                  // Serial.printf("found '%c' at %d. Slicing out %.4lf\n", ch, j, x);
                  i = j + 1;
                }
                if (ch == '\n' || ch == '\0')
                {
                  y = body.substring(i, j).toDouble();
                  // Serial.printf("found '%c' at %d. Slicing out %.4lf\n", ch, j, y);
                  i = j + 1;
                  Serial.printf("%lf,%lf\n", x, y);
                }
                j++;
              }
              // Create a buffer to store the body data as a string
              // char *buf = (char*)malloc(len + 1);
              // memcpy(buf, data, len);
              // buf[len] = '\0';

              // Print the received data
              // Serial.print("Received body data: ");
              // Serial.println(buf);
              // Serial.print("Chunk index: ");
              // Serial.println(index);
              // Serial.print("Total length: ");
              // Serial.println(total);

              // free(buf);
            });

  // Start server
  server.begin();
}

void loop() {}
