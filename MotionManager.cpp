#include "MotionManager.h"
#include <Arduino.h>

MotionManager::MotionManager(int sensorPin) {
    vibrationPin = sensorPin;
}

void MotionManager::setup() {
    pinMode(vibrationPin, INPUT);
    // Lecture initiale pour définir l'état de base
    lastState = digitalRead(vibrationPin);
}

bool MotionManager::checkVibration() {
    bool currentState = digitalRead(vibrationPin);
    
    // Une vibration est détectée lorsque l'état change
    if (currentState != lastState) {
        lastState = currentState;
        // Le capteur passe généralement à LOW en cas de vibration
        return !currentState;
    }
    return false;
}