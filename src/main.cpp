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
#define STEPS_PER_RADIAN -STEPS_PER_REV / (2 * PI) * STEPPER_GEAR_RATIO // approx -57

// Create stepper objects
AccelStepper stepper1(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper stepper2(AccelStepper::DRIVER, stepPin2, dirPin2);
MultiStepper steppers;

long *angles_to_steps(MotorAngles angles)
{
  int pos_1 = round(angles.a1 * STEPS_PER_RADIAN);

  // counteract drift
  float delta1 = pos_1 - stepper1.currentPosition();
  int arm_2_drift = round(delta1 / STEPPER_GEAR_RATIO);
  float pos_2 = stepper2.currentPosition() + arm_2_drift;

  // apply desired position (relative to arm 1)
  pos_2 += round(angles.a2 * STEPS_PER_RADIAN);

  static long steps[2];
  steps[0] = pos_1;
  steps[1] = pos_2;

  return steps;
}

void go_to(int x, int y)
{
  Serial.printf("go_to(%d, %d)\n", x, y);
  MotorAngles angles = coord_to_angles(x, y);
  Serial.printf("\tneed angles (%.2fπ, %.2fπ)\n", angles.a1 / PI, angles.a2 / PI);
  long *steps = angles_to_steps(angles); // problem lies here

  steppers.moveTo(steps);
  steppers.runSpeedToPosition();
}

void execute()
{
  go_to(15, 15);
  delay(3000);
  go_to(0, 30);
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

  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  Serial.println("\n\n🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸\n\n");

  Serial.println("done setup, executing movements");
  execute();
  Serial.println("done executing movements.");
  delay(2000);

  Serial.println("resetting");
  long steps[2];
  steps[0] = 0;
  steps[1] = 0;
  steppers.moveTo(steps);
  steppers.runSpeedToPosition();
  Serial.println("\n\n🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸\n\n");
}

void loop() {}
