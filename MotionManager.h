#ifndef MOTION_MANAGER_H
#define MOTION_MANAGER_H

#include <Arduino.h>

class MotionManager {
public:
    MotionManager(int sensorPin);
    void setup();
    bool checkVibration();

private:
    int vibrationPin;
    bool lastState = false;
};

#endif