#include <Arduino.h>
#include <stdio.h>

typedef struct
{
    float x;
    float y;
} Coordinate;

typedef struct
{
    float a1;
    float a2;
} MotorAngles;

typedef struct
{
    float s1;
    float s2;
} MotorSteps;

MotorAngles coord_to_angles(int, int);
int positiveMod(int, int);
