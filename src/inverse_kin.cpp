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

int L = 15; // cm


MotorAngles coord_to_angles(Coordinate c){
    float r_2 = c.x * c.x + c.y * c.y;
    MotorAngles res;

    float c2 = (r_2 - 2 * L * L) / (2 * L * L);
    res.t2 = acos(c2);
    float theta = atan2(c.y, c.x);
    res.t1 = theta - atan2(L * sin(res.t2), L + L * c2);
    
    
    return res;
}

int main(){
    Coordinate target;
    target.x = 30;
    target.y = 0;

    MotorAngles result = coord_to_angles(target);

    printf("%f\n", result.t1);
    printf("%f\n", result.t2);
}