#ifndef MOTION_MANAGER_H
#define MOTION_MANAGER_H

#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class MotionManager {
public:
    MotionManager();
    void setup();
    bool checkVibration();

private:
    Adafruit_MMA8451 mma;
    float previousAcceleration;
};

#endif