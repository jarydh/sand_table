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

void set_target_position(int x, int y)
{
  Coordinate coord;
  coord.x = x;
  coord.y = y;
  MotorAngles angles = coord_to_angles(coord);
  Serial.printf("angle 1: %f\n", angles.t1);
  Serial.printf("angle 2: %f\n", angles.t2);

  int pos_1 = round(angles.t1 * STEPS_PER_RADIAN);

  // counteract drift
  float delta1 = pos_1 - stepper1.currentPosition();
  int arm_2_drift = round(delta1 / STEPPER_GEAR_RATIO);
  float pos_2 = stepper2.currentPosition() + arm_2_drift;

  Serial.printf("pos_2 drift compensation: %d\n", pos_2);

  // apply desired position (relative to arm 1)
  pos_2 += round(angles.t2 * STEPS_PER_RADIAN);

  Serial.printf("pos_1 final: %d\n", pos_1);
  Serial.printf("pos_2 final: %d\n", pos_2);

  long steps[2];
  steps[0] = pos_1;
  steps[1] = pos_2;
  steppers.moveTo(steps);
}

void execute()
{
  set_target_position(0, 30);
  steppers.runSpeedToPosition();

  delay(2000);

  set_target_position(15, 15);
  steppers.runSpeedToPosition();
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

  Serial.println("done setup, executing movements in 2 seconds:");

  delay(2000);

  execute();

  Serial.println("done executing movements.");
}

void loop() {}
