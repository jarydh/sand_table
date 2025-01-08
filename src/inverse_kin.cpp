#include "inverse_kin.h"

int L = 15; // cm

// returns angles in degrees
MotorAngles coord_to_angles(int x, int y)
{
    x = -x;
    float r_2 = x * x + y * y;
    MotorAngles res;

    float c2 = (r_2 - 2 * L * L) / (2 * L * L);
    res.a2 = acos(c2);
    float theta = atan2(y, x);
    res.a1 = theta - atan2(L * sin(res.a2), L + L * c2);

    res.a1 *= 180 / PI;
    res.a2 *= 180 / PI;
    return res;
}

int positiveMod(int a, int b)
{
    return ((a % b) + b) % b;
}