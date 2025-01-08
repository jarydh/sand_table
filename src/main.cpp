#include <AccelStepper.h>
#include <MultiStepper.h>
#include "inverse_kin.h"

#define LED_PIN 22
#define STEP_PIN_1 12
#define DIR_PIN_1 14
#define STEP_PIN_2 26
#define DIR_PIN_2 27
#define SERIAL_PORT 115200

#define STEPS_PER_REV 200
#define STEPPER_GEAR_RATIO 1.8
#define STEPS_PER_RADIAN -STEPS_PER_REV / (2 * PI) * STEPPER_GEAR_RATIO // approx -57

#define START_POSITION 30, 0

AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);
MultiStepper steppers;

long *angles_to_steps(MotorAngles angles)
{
  long pos_1 = round(angles.a1 * STEPS_PER_RADIAN);
  long pos_2 = round(angles.a2 * STEPS_PER_RADIAN + pos_1 / STEPPER_GEAR_RATIO);

  static long steps[2];
  steps[0] = pos_1;
  steps[1] = pos_2;

  return steps;
}

// sends the arm head to the x,y position
void go_to(int x, int y)
{
  Serial.printf("go to (%d, %d)\n", x, y);
  MotorAngles angles = coord_to_angles(x, y);
  Serial.printf("\tneed angles (%.2fπ, %.2fπ)\n", angles.a1 / PI, angles.a2 / PI);
  long *steps = angles_to_steps(angles);

  Serial.printf("\tsend arm 1 to %d\n", steps[0]);
  Serial.printf("\tsend arm 2 to %d\n", steps[1]);

  steppers.moveTo(steps);
  steppers.runSpeedToPosition();
}

// executed on startup after setup() as a script
void execute()
{
  go_to(-15, -15);
  delay(2000);
  go_to(15, -15);
  delay(2000);
  go_to(0, 0);
  delay(2000);
  go_to(0, 15);
  delay(2000);
  go_to(-15, 0);
  delay(2000);
  go_to(0, -15);
}

void setup()
{
  Serial.begin(SERIAL_PORT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  pinMode(STEP_PIN_2, OUTPUT);
  pinMode(DIR_PIN_2, OUTPUT);

  stepper1.setMaxSpeed(50);
  stepper1.setSpeed(50);
  stepper2.setMaxSpeed(50);
  stepper2.setSpeed(50);

  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  Serial.println("\n\n🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸\n\n");
  Serial.println("done setup, executing movements");

  execute();

  Serial.println("done executing movements.");
  delay(2000);
  Serial.println("resetting");
  go_to(START_POSITION);
  Serial.println("\n\n🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸🐸\n\n");
}

void loop() {}
