#include <Arduino.h>
#include <stdio.h>
#include <cmath>

typedef struct
{
    double x;
    double y;
} Coordinate;

typedef struct
{
    double a1;
    double a2;
} MotorAngles;

typedef struct
{
    double s1;
    double s2;
} MotorSteps;

MotorAngles coord_to_angles(int, int);
double positiveMod(double, double);
