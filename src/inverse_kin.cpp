#include "inverse_kin.h"

double L = 0.5; // unitless

double positiveMod(double a, double b)
{
    return fmod(fmod(a, b) + b, b);
}

// returns angles in degrees
MotorAngles coord_to_angles(double x, double y)
{
    x = -x;
    double r_2 = x * x + y * y;
    MotorAngles res;

    double c2 = (r_2 - 2 * L * L) / (2 * L * L);
    res.a2 = acos(c2);
    double theta = atan2(y, x);
    res.a1 = theta - atan2(L * sin(res.a2), L + L * c2);

    res.a1 *= 180 / PI;
    res.a2 *= 180 / PI;

    res.a1 = positiveMod(res.a1, 360);
    res.a2 = positiveMod(res.a2, 360);
    return res;
}
