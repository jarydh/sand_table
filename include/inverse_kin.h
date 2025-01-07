#include <Arduino.h>
#include <stdio.h>

typedef struct
{
    float x;
    float y;
} Coordinate;

typedef struct
{
    float t1;
    float t2;
} MotorAngles;

typedef struct
{
    float s1;
    float s2;
} MotorSteps;

MotorAngles coord_to_angles(Coordinate c);
