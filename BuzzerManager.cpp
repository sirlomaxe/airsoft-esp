#include "BuzzerManager.h"

BuzzerManager::BuzzerManager(int buzzerPin) : buzzerPin(buzzerPin) {}

void BuzzerManager::setup() {
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
}

void BuzzerManager::startBeeping(int duration) {
    if (duration > 0) {
        digitalWrite(buzzerPin, HIGH);
        beepEndTime = millis() + duration;
    }
}

void BuzzerManager::playSuccess() {
    // Exemple de son de succès (court et aigu)
    tone(buzzerPin, 2000, 200); // Fréquence 2000 Hz pendant 200 ms
    delay(200); // S'assurer que le son a le temps de jouer avant de passer à autre chose
    digitalWrite(buzzerPin, LOW);
}

void BuzzerManager::playHit() {
    // Exemple de son de "hit" (court et grave)
    tone(buzzerPin, 500, 150); // Fréquence 500 Hz pendant 150 ms
    delay(150);
    digitalWrite(buzzerPin, LOW);
}

void BuzzerManager::playDominationSwitch() {
    // Son pour indiquer un changement d'équipe (modulé)
    tone(buzzerPin, 1000, 100);
    delay(100);
    tone(buzzerPin, 1200, 100);
    delay(100);
    digitalWrite(buzzerPin, LOW);
}

// Dans la loop() principale de votre GameManager, vous devrez gérer la fin du bip
// quand beepEndTime est dépassé.