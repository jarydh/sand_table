#include "inverse_kin.h"

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