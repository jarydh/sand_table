#include "SPIFFS.h"
#include <AccelStepper.h>
#include <MultiStepper.h>
#include "stepper_control.h"
#include "inverse_kin.h"

#include "WiFi.h"
#include "WifiManager.h"
#include "ESPAsyncWebServer.h"

#define LED_PIN 22
#define STEP_PIN_1 12
#define DIR_PIN_1 14
#define STEP_PIN_2 26
#define DIR_PIN_2 27
#define SERIAL_PORT 115200

#define STEPS_PER_REV 200
#define STEPPER_GEAR_RATIO 1.8
#define STEPS_PER_REV 200
#define STEPPER_GEAR_RATIO 1.8
#define START_POSITION -1, 0

AsyncWebServer server(80);

AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);
MultiStepper steppers;

long *angles_to_steps(MotorAngles angles)
{
  // recalculate a1 to the closest equivalent angle
  if (angles.a1 - stepper1.currentPosition() > 180)
  {
    angles.a1 -= 360;
  }
  if (stepper1.currentPosition() - angles.a1 > 180)
  {
    angles.a1 += 360;
  }
  assert(abs(stepper1.currentPosition() - angles.a1) <= 180);

  long pos_1 = round(angles.a1);
  long delta = pos_1 - stepper1.currentPosition();

  double drift = delta / STEPPER_GEAR_RATIO;

  long pos_2 = round(drift + angles.a2);

  static long steps[2];
  steps[0] = pos_1;
  steps[1] = pos_2;

  return steps;
}

// sends the arm head to the x, y position
void go_to(double x, double y)
{
  Serial.printf("go to [%0.2f, %0.2f]\n", x, y);
  MotorAngles angles = coord_to_angles(x, y);
  Serial.printf("\tneed angles: %.0f°, %.0f°\n", angles.a1, angles.a2);
  long *steps = angles_to_steps(angles);

  Serial.printf("\tsend arm 1 to %d\n", steps[0]);
  Serial.printf("\tsend arm 2 to %d\n", steps[1]);

  steppers.moveTo(steps);
  steppers.runSpeedToPosition();

  // reset the motors frame of reference
  stepper1.setCurrentPosition(round(positiveMod(angles.a1, 360)));
  stepper2.setCurrentPosition(round(positiveMod(angles.a2, 360)));
}

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
                  i = j + 1;
                }
                if (ch == '\n' || ch == '\0')
                {
                  y = body.substring(i, j).toDouble();
                  i = j + 1;
                  Serial.printf("%lf,%lf\n", x, y);
                  go_to(x, y);
                  delay(500);
                }
                j++;
              }
              go_to(START_POSITION); });

  // Start server
  server.begin();

  // stepper stuff
  //
  //
  //
  stepper1.setMaxSpeed(80);
  stepper1.setSpeed(80);
  stepper2.setMaxSpeed(80);
  stepper2.setSpeed(80);

  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);
}

void loop() {}
