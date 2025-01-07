/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Import required libraries
// #include "WiFi.h"
// #include "WifiManager.h"
// #include "ESPAsyncWebServer.h"
// #include "SPIFFS.h"
#include <AccelStepper.h>
#include <MultiStepper.h>

#include "inverse_kin.h"

// Set LED GPIO
const int ledPin = 22;
// Stores LED state
String ledState;

// Set stepper motor pins
const int stepPin1 = 12;
const int dirPin1 = 14;
const int stepPin2 = 26;
const int dirPin2 = 27;

// Stepper motor configuration
#define STEPS_PER_REV 200
#define STEPPER_GEAR_RATIO 1.8

// Create stepper objects
AccelStepper stepper1(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper stepper2(AccelStepper::DRIVER, stepPin2, dirPin2);

MultiStepper steppers;

// // Create AsyncWebServer object on port 80
// AsyncWebServer server(80);

// Replaces placeholder with LED state value
// String processor(const String &var)
// {
//   Serial.println(var);
//   if (var == "STATE")
//   {
//     if (digitalRead(ledPin))
//     {
//       ledState = "ON";
//     }
//     else
//     {
//       ledState = "OFF";
//     }
//     Serial.print(ledState);
//     return ledState;
//   }
//   return String();
// }

void set_target_position(MotorAngles angles)
{
  // add 0.5 for rounding
  int steps_1 = -(angles.t1 * 180 / PI * STEPS_PER_REV * STEPPER_GEAR_RATIO / 360 + 0.5);
  int steps_2_compensation = (steps_1 / STEPPER_GEAR_RATIO + 0.5);
  int steps_2_position = -(angles.t2 * 180 / PI * STEPS_PER_REV * STEPPER_GEAR_RATIO / 360 + 0.5);

  int steps_2 = steps_2_position + steps_2_compensation;
  Serial.println(steps_1);

  Serial.printf("steps_1: %d\n", steps_1);
  Serial.printf("steps_2: %d\n", steps_2);

  long steps[2];
  steps[0] = steps_1;
  steps[1] = steps_2;
  steppers.moveTo(steps);
}

void new_coordinate(int x, int y){
    Coordinate coord;
    coord.x = x;
    coord.y = y;
    Serial.println(coord_to_angles(coord).t1);
    Serial.println(coord_to_angles(coord).t2);

    set_target_position(coord_to_angles(coord));
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);

  stepper1.setMaxSpeed(50);
  stepper1.moveTo(STEPS_PER_REV * STEPPER_GEAR_RATIO);
  stepper1.setAcceleration(10);

  stepper2.setMaxSpeed(50);
  stepper2.moveTo(STEPS_PER_REV * STEPPER_GEAR_RATIO);
  stepper2.setAcceleration(10);
  // stepper1.setSpeed(50);


  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);
}

void loop()
{
  new_coordinate(0, 30);
  steppers.runSpeedToPosition();

  delay(2000);

  new_coordinate(-15, 0);
  steppers.runSpeedToPosition();

  delay(2000);


  new_coordinate(30, 0);
  steppers.runSpeedToPosition();

  delay(2000);
}
