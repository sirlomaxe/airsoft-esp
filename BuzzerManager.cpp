#include "BuzzerManager.h"

BuzzerManager::BuzzerManager(int buzzerPin) : buzzerPin(buzzerPin) {}

void BuzzerManager::setup() {
    pinMode(buzzerPin, OUTPUT);
}

void BuzzerManager::playBeep() {
    tone(buzzerPin, 1000, 100);
}

void BuzzerManager::playExplosion() {
    // Joue une explosion, un son plus complexe
    tone(buzzerPin, 200, 500);
    delay(500);
    tone(buzzerPin, 150, 500);
    delay(500);
    noTone(buzzerPin);
}

void BuzzerManager::playDefusal() {
    // Joue un son de désamorçage réussi
    tone(buzzerPin, 500, 200);
    delay(200);
    tone(buzzerPin, 800, 200);
    delay(200);
    tone(buzzerPin, 1200, 400);
    noTone(buzzerPin);
}