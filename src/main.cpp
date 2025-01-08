#include <AccelStepper.h>
#include <MultiStepper.h>
#include "inverse_kin.h"
#include "SPIFFS.h"

#define LED_PIN 22
#define STEP_PIN_1 12
#define DIR_PIN_1 14
#define STEP_PIN_2 26
#define DIR_PIN_2 27
#define SERIAL_PORT 115200

#define STEPS_PER_REV 200
#define STEPPER_GEAR_RATIO 1.8

#define START_POSITION -1, 0

AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);
MultiStepper steppers;

long *angles_to_steps(MotorAngles angles)
{
  // recalculate the a1 to the closest
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

// sends the arm head to the x,y position
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

  stepper1.setCurrentPosition(positiveMod(angles.a1, 360));
  stepper2.setCurrentPosition(positiveMod(angles.a2, 360));

  // Serial.printf("\tstepper1.currentPosition(): %d\n", stepper1.currentPosition());
  // Serial.printf("\tstepper2.currentPosition(): %d\n", stepper2.currentPosition());
}

// executed on startup after setup() as a script
void execute()
{
  File f = SPIFFS.open("/positions.in");

  while (f.available())
  {
    String line = f.readStringUntil('\n');
    double x, y;

    if (sscanf(line.c_str(), "%lf,%lf", &x, &y) == 2)
    {
      go_to(x, y);
      delay(1000);
    }
    else
    {
      Serial.printf("Skipping invalid line: %s\n", line.c_str());
    }
  }
  f.close();
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
    return;
  }

  stepper1.setMaxSpeed(80);
  stepper1.setSpeed(80);
  stepper2.setMaxSpeed(80);
  stepper2.setSpeed(80);

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
