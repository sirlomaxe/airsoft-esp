#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include <Arduino.h>

class BuzzerManager {
public:
    BuzzerManager(int buzzerPin);
    void setup();
    void playBeep();
    void playExplosion();
    void playDefusal();

private:
    int buzzerPin;
};

#endif