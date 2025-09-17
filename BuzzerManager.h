#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include <Arduino.h>

class BuzzerManager {
public:
    BuzzerManager(int buzzerPin);
    void setup();
    void startBeeping(int duration); // Durée en ms
    void playSuccess();
    void playHit();
    void playDominationSwitch(); // Nouvelle fonction pour le mode domination

private:
    int buzzerPin;
    unsigned long beepEndTime = 0;
};

#endif